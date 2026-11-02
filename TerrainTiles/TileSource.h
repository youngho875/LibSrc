#pragma once
#include <cpl_json.h>
#include <cpl_http.h>
#include <cpl_vsi.h>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
namespace tiles {
constexpr float missing=-3.4e38f;
struct Grid { static constexpr int N=129;std::vector<float> h=std::vector<float>(N*N,missing); };
struct Reader {
 const std::vector<unsigned char>& b;size_t p=0;
 template<class T>T get(){if(p>b.size()||sizeof(T)>b.size()-p)throw std::runtime_error("truncated terrain");T v;memcpy(&v,b.data()+p,sizeof(v));p+=sizeof(v);return v;}
 void skip(size_t n){if(p>b.size()||n>b.size()-p)throw std::runtime_error("truncated terrain");p+=n;}
};
inline std::shared_ptr<Grid> Decode(const std::vector<unsigned char>& bytes,bool heightmap){
 auto g=std::make_shared<Grid>();Reader r{bytes};
 if(heightmap){if(bytes.size()!=8452&&bytes.size()!=8452+256*256-1)throw std::runtime_error("invalid heightmap length");
  std::vector<float> h(65*65);for(auto&v:h)v=r.get<uint16_t>()*.2f-1000.f;
  for(int y=0;y<Grid::N;++y)for(int x=0;x<Grid::N;++x){double px=x*.5,py=(128-y)*.5;int ix=std::min(63,(int)px),iy=std::min(63,(int)py);double a=px-ix,b=py-iy;g->h[y*Grid::N+x]=(float)((1-b)*((1-a)*h[iy*65+ix]+a*h[iy*65+ix+1])+b*((1-a)*h[(iy+1)*65+ix]+a*h[(iy+1)*65+ix+1]));}return g;
 }
 r.skip(24);float lo=r.get<float>(),hi=r.get<float>();if(!std::isfinite(lo)||!std::isfinite(hi)||hi<lo)throw std::runtime_error("invalid heights");r.skip(56);
 uint32_t n=r.get<uint32_t>();if(n<3||n>1000000||n>(bytes.size()-r.p)/6)throw std::runtime_error("invalid vertex count");
 std::vector<double> u(n),v(n),h(n);for(auto arr:{&u,&v,&h}){int value=0;for(auto&out:*arr){auto z=r.get<uint16_t>();value+=(z>>1)^-(int)(z&1);if(value<0||value>32767)throw std::runtime_error("invalid vertex delta");out=value;}}
 for(uint32_t i=0;i<n;++i){u[i]*=128./32767;v[i]*=128./32767;h[i]=lo+(hi-lo)*h[i]/32767.;}
 size_t width=n>65536?4:2;r.skip((width-r.p%width)%width);uint32_t triangles=r.get<uint32_t>();if(triangles>2000000||triangles>(bytes.size()-r.p)/(width*3))throw std::runtime_error("invalid triangle count");
 uint64_t work=0;uint32_t highest=0;for(uint32_t t=0;t<triangles;++t){uint32_t idx[3];for(auto&i:idx){uint32_t code=width==4?r.get<uint32_t>():r.get<uint16_t>();if(code>highest)throw std::runtime_error("invalid triangle code");i=highest-code;if(!code)++highest;if(i>=n)throw std::runtime_error("invalid triangle index");}
  auto a=idx[0],b=idx[1],c=idx[2];double det=(v[b]-v[c])*(u[a]-u[c])+(u[c]-u[b])*(v[a]-v[c]);if(fabs(det)<1e-12)continue;
  int x0=std::max(0,(int)ceil(std::min({u[a],u[b],u[c]}))),x1=std::min(128,(int)floor(std::max({u[a],u[b],u[c]})));
  int y0=std::max(0,(int)ceil(std::min({v[a],v[b],v[c]}))),y1=std::min(128,(int)floor(std::max({v[a],v[b],v[c]})));
  work+=(uint64_t)(std::max)(0,x1-x0+1)*(std::max)(0,y1-y0+1);if(work>50000000)throw std::runtime_error("terrain raster work limit");
  for(int y=y0;y<=y1;++y)for(int x=x0;x<=x1;++x){double wa=((v[b]-v[c])*(x-u[c])+(u[c]-u[b])*(y-v[c]))/det,wb=((v[c]-v[a])*(x-u[c])+(u[a]-u[c])*(y-v[c]))/det,wc=1-wa-wb;if(wa>=-1e-8&&wb>=-1e-8&&wc>=-1e-8)g->h[y*Grid::N+x]=(float)(wa*h[a]+wb*h[b]+wc*h[c]);}
 }
 for(int edge=0;edge<4;++edge){uint32_t count=r.get<uint32_t>();if(count>n)throw std::runtime_error("invalid edge count");for(uint32_t i=0;i<count;++i)if((width==4?r.get<uint32_t>():r.get<uint16_t>())>=n)throw std::runtime_error("invalid edge index");}
 while(r.p<bytes.size()){r.get<uint8_t>();r.skip(r.get<uint32_t>());}return g;
}
inline bool Http(const std::string&s){return s.rfind("http://",0)==0||s.rfind("https://",0)==0;}
inline void Replace(std::string&s,const std::string&a,const std::string&b){size_t p=0;while((p=s.find(a,p))!=std::string::npos){s.replace(p,a.size(),b);p+=b.size();}}
struct Source {
 std::string base,pattern,version,error;bool server=false,heightmap=false,mercator=false,slippy=false;int minzoom=0,maxzoom=14;CPLJSONArray available;
 std::atomic<bool>&stop;std::map<std::string,std::shared_ptr<Grid>> cache;std::vector<std::string> order;bool ready=false,incomplete=false;int failures=0,budget=96;std::chrono::steady_clock::time_point deadline=std::chrono::steady_clock::time_point::max();
 explicit Source(std::atomic<bool>&s):stop(s){}
 struct Download {std::vector<unsigned char> bytes;std::atomic<bool>*stop;};
 static size_t Write(void* p,size_t s,size_t n,void* arg){auto&d=*(Download*)arg;if(*d.stop||s&&n>16*1024*1024/s)return 0;size_t len=s*n;if(len>16*1024*1024-d.bytes.size())return 0;try{auto b=(unsigned char*)p;d.bytes.insert(d.bytes.end(),b,b+len);return len;}catch(...){return 0;}}
 std::vector<unsigned char> Read(const std::string&location){
  std::vector<unsigned char>b;
  if(server){if(!Http(location))throw std::runtime_error("server tiles require HTTP(S)");Download d{{},&stop};char**opt=nullptr;opt=CSLSetNameValue(opt,"TIMEOUT","3");opt=CSLSetNameValue(opt,"CONNECTTIMEOUT","2");opt=CSLSetNameValue(opt,"MAX_RETRY","0");opt=CSLSetNameValue(opt,"HEADERS","Accept: application/vnd.quantized-mesh,application/octet-stream;q=0.9");CPLPushErrorHandler(CPLQuietErrorHandler);auto res=CPLHTTPFetchEx(location.c_str(),opt,nullptr,nullptr,Write,&d);CPLPopErrorHandler();CSLDestroy(opt);bool ok=res&&res->nStatus==0&&!res->pszErrBuf;CPLHTTPDestroyResult(res);if(!ok)throw std::runtime_error("terrain HTTP request failed");b=std::move(d.bytes);
  }else{auto f=VSIFOpenL(location.c_str(),"rb");if(!f)throw std::runtime_error("terrain file missing");unsigned char chunk[8192];while(!stop){size_t n=VSIFReadL(chunk,1,sizeof(chunk),f);if(!n)break;if(b.size()+n>16*1024*1024){VSIFCloseL(f);throw std::runtime_error("terrain file too large");}b.insert(b.end(),chunk,chunk+n);}VSIFCloseL(f);}
  if(b.size()>2&&b[0]==31&&b[1]==139){static std::atomic<uint64_t> serial{0};std::string name="/vsimem/c4-terrain-"+std::to_string((uintptr_t)this)+"-"+std::to_string(++serial)+".gz";auto f=VSIFileFromMemBuffer(name.c_str(),b.data(),b.size(),FALSE);if(f)VSIFCloseL(f);auto gz=VSIFOpenL(("/vsigzip/"+name).c_str(),"rb");std::vector<unsigned char>out;if(gz){unsigned char chunk[8192];while(!stop){size_t n=VSIFReadL(chunk,1,sizeof(chunk),gz);if(!n)break;if(out.size()+n>32*1024*1024){VSIFCloseL(gz);VSIUnlink(name.c_str());throw std::runtime_error("expanded terrain too large");}out.insert(out.end(),chunk,chunk+n);}VSIFCloseL(gz);}VSIUnlink(name.c_str());if(out.empty())throw std::runtime_error("invalid gzip terrain");return out;}return b;
 }
 std::string Resolve(const std::string&s){if(server){if(Http(s))return s;if(s.rfind("//",0)==0)return base.substr(0,base.find(':')+1)+s;if(!s.empty()&&s[0]=='/'){auto pos=base.find('/',base.find("://")+3);return base.substr(0,pos)+s;}return base+s;}
  if(Http(s)||s.empty()||s[0]=='/'||s.find(':')!=std::string::npos)throw std::runtime_error("local tiles require relative paths");auto p=std::filesystem::u8path(s.substr(0,s.find_first_of("?#")));for(const auto&part:p)if(part=="..")throw std::runtime_error("tile path escapes root");return (std::filesystem::u8path(base)/p).u8string();
 }
 void Open(const std::string&path,bool remote){server=remote;if(server&&!Http(path))throw std::runtime_error("terrain server requires HTTP(S) layer.json URL");if(!server&&Http(path))throw std::runtime_error("local terrain requires a filesystem path");std::string metadata=path;
  auto at=metadata.find("layer.json");if(at!=std::string::npos){base=metadata.substr(0,at);}else{base=metadata;if(base.back()!='/'&&base.back()!='\\')base+='/';metadata=base+"layer.json";}
  auto bytes=Read(metadata);CPLJSONDocument doc;if(bytes.empty()||!doc.LoadMemory(std::string(bytes.begin(),bytes.end())))throw std::runtime_error("invalid layer.json");auto root=doc.GetRoot();auto format=root.GetString("format");heightmap=format=="heightmap-1.0";if(!heightmap&&format!="quantized-mesh-1.0")throw std::runtime_error("unsupported terrain format");auto projection=root.GetString("projection","EPSG:4326");mercator=projection=="EPSG:3857";if(!mercator&&projection!="EPSG:4326")throw std::runtime_error("unsupported terrain projection");auto scheme=root.GetString("scheme","tms");slippy=scheme=="slippyMap";if(!slippy&&scheme!="tms")throw std::runtime_error("unsupported terrain scheme");auto templates=root.GetArray("tiles");if(templates.Size()<1)throw std::runtime_error("missing terrain tile template");pattern=templates[0].ToString();version=root.GetString("version","1.0.0");minzoom=root.GetInteger("minzoom",0);maxzoom=root.GetInteger("maxzoom",14);if(minzoom<0||maxzoom<minzoom||maxzoom>24)throw std::runtime_error("invalid terrain zoom range");if(!root.GetString("parentUrl").empty())throw std::runtime_error("parent terrain layers unsupported");available=root.GetArray("available");Resolve(pattern);ready=true;
 }
 bool Available(int z,int x,int y){if(z<minzoom||z>maxzoom)return false;if(available.Size()==0)return true;if(z>=available.Size())return false;auto ranges=available[z].ToArray();for(int i=0;i<ranges.Size();++i){auto r=ranges[i];if(x>=r.GetInteger("startX")&&x<=r.GetInteger("endX")&&y>=r.GetInteger("startY")&&y<=r.GetInteger("endY"))return true;}return false;}
 std::shared_ptr<Grid> Load(int z,int x,int y){if(!Available(z,x,y)||stop)return {};std::string key=std::to_string(z)+"/"+std::to_string(x)+"/"+std::to_string(y);auto found=cache.find(key);if(found!=cache.end())return found->second;if(budget<=0||std::chrono::steady_clock::now()>deadline){incomplete=true;return {};}--budget;std::string name=pattern;Replace(name,"{z}",std::to_string(z));Replace(name,"{x}",std::to_string(x));Replace(name,"{y}",std::to_string(slippy?(1<<z)-1-y:y));Replace(name,"{version}",version);std::shared_ptr<Grid>g;
  try{g=Decode(Read(Resolve(name)),heightmap);}catch(const std::exception&e){error=e.what();++failures;}
  if(cache.size()>=128){cache.erase(order.front());order.erase(order.begin());}cache[key]=g;order.push_back(key);return g;
 }
 double Y(double lat)const{if(!mercator)return (lat+90)/180;lat=std::clamp(lat,-85.0511287798,85.0511287798);return (1+log(tan(3.141592653589793/4+lat*3.141592653589793/360))/3.141592653589793)/2;}
 bool Sample(double lon,double lat,int zoom,float&h){lon=fmod(lon+180,360);if(lon<0)lon+=360;double ux=lon/360,uy=std::clamp(Y(lat),0.,std::nextafter(1.,0.));if(mercator&&fabs(lat)>85.0511287798)return false;
  for(int z=std::min(zoom,maxzoom);z>=minzoom&&!stop;--z){int nx=(mercator?1:2)*(1<<z),ny=1<<z;double x=ux*nx,y=uy*ny;int tx=(int)x,ty=(int)y;auto g=Load(z,tx,ty);if(!g)continue;double px=(x-tx)*128,py=(y-ty)*128;if(mercator&&!heightmap){auto latitude=[](double a){return atan(sinh((2*a-1)*3.141592653589793))*180/3.141592653589793;};double south=latitude((double)ty/ny),north=latitude((double)(ty+1)/ny);py=std::clamp((lat-south)/(north-south)*128,0.,128.);}int ix=std::min(127,(int)px),iy=std::min(127,(int)py);double a=px-ix,b=py-iy;float v[4]={g->h[iy*129+ix],g->h[iy*129+ix+1],g->h[(iy+1)*129+ix],g->h[(iy+1)*129+ix+1]};if(std::find(v,v+4,missing)!=v+4)continue;h=(float)((1-b)*((1-a)*v[0]+a*v[1])+b*((1-a)*v[2]+a*v[3]));return true;}return false;
 }
};
}
