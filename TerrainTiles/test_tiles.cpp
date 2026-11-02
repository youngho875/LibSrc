#include "TerrainTiles.h"
#include "TileSource.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <functional>
void Check(bool ok,const char*what){if(!ok)throw std::runtime_error(what);}
void Wait(const std::function<bool()>&fn){for(int i=0;i<1000;++i){if(fn())return;std::this_thread::sleep_for(std::chrono::milliseconds(10));}throw std::runtime_error("timeout");}
int main(int argc,char**argv){try{Check(argc>=2,"fixture root required");bool remote=argc>2;std::string root=argv[1];
 for(auto name:{"mesh","mesh32","heightmap","slippy"}){auto handle=TerrainTilesCreate((root+"/"+name+"/layer.json").c_str(),remote?1:0);Check(handle,"create");TerrainTilesStatus s{};s.size=sizeof(s);Wait([&]{TerrainTilesGetStatus(handle,&s);return !s.indexing;});char error[512];TerrainTilesGetError(handle,error,sizeof(error));if(s.rejected)std::cerr<<error<<'\n';Check(s.files==1,"metadata");
  auto ticket=TerrainTilesRequestSample(handle,45,45);double h;Wait([&]{return TerrainTilesGetSample(handle,ticket,&h)!=0;});Check(TerrainTilesGetSample(handle,ticket,&h)==1,"height available");Check(fabs(h-(std::string(name)=="heightmap"?400:200))<.1,"height exact");
  TerrainTilesView v{0,0,90,90,128,128};auto frame=TerrainTilesRequestView(handle,&v);std::vector<unsigned char>pixels(128*128*4);Wait([&]{return TerrainTilesCopyFrame(handle,frame,pixels.data(),pixels.size())==1;});for(size_t i=3;i<pixels.size();i+=4)Check(pixels[i]==255,"frame coverage");Check(TerrainTilesCopyFrame(handle,frame,pixels.data(),4)==-1,"short buffer");
  v.west=360;v.east=450;auto wrapped=TerrainTilesRequestView(handle,&v);Check(TerrainTilesCopyFrame(handle,frame,pixels.data(),pixels.size())==0,"stale frame");Wait([&]{return TerrainTilesCopyFrame(handle,wrapped,pixels.data(),pixels.size())==1;});Check(pixels[3]==255,"wrap frame");
  ticket=TerrainTilesRequestSample(handle,-45,-45);Wait([&]{return TerrainTilesGetSample(handle,ticket,&h)!=0;});Check(TerrainTilesGetSample(handle,ticket,&h)==2,"missing tile");Check(TerrainTilesRequestSample(handle,0,91)==0,"latitude validation");TerrainTilesDestroy(handle);std::cout<<"PASS "<<name<<(remote?" HTTP":" local")<<" height, gzip, image, wrap, stale, no data\n";
 }
 bool rejected=false;try{tiles::Decode(std::vector<unsigned char>(100,0),false);}catch(...){rejected=true;}Check(rejected,"malformed tile rejected");
 auto bad=TerrainTilesCreate((root+"/absent/layer.json").c_str(),remote?1:0);TerrainTilesStatus status{};status.size=sizeof(status);Wait([&]{TerrainTilesGetStatus(bad,&status);return !status.indexing;});char error[512];Check(TerrainTilesGetError(bad,error,sizeof(error))>0,"metadata diagnostics");TerrainTilesDestroy(bad);
 std::cout<<"PASS malformed input and metadata diagnostics\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
