#include "TerrainElevation.h"
#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <filesystem>
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <stdexcept>
void Check(bool ok,const char*m){if(!ok)throw std::runtime_error(m);}
void WaitIndex(TerrainHandle h){for(int i=0;i<500;++i){TerrainStatus s{};s.size=sizeof(s);Check(TerrainGetStatus(h,&s)==1,"status");if(!s.indexing){Check(s.files==2,"recursive file count");return;}std::this_thread::sleep_for(std::chrono::milliseconds(20));}throw std::runtime_error("index timeout");}
int WaitSample(TerrainHandle h,uint64_t ticket,double&value){for(int i=0;i<500;++i){int result=TerrainGetSample(h,ticket,&value);if(result)return result;std::this_thread::sleep_for(std::chrono::milliseconds(20));}throw std::runtime_error("sample timeout");}
int main(int argc,char**argv){TerrainHandle h=nullptr;std::filesystem::path root=std::filesystem::temp_directory_path()/"c4-terrain-sdk-test";try{
 Check(!std::filesystem::exists(root),"test path already exists");std::filesystem::create_directories(root/"a/b/c/d");GDALAllRegister();OGRSpatialReference sr;sr.SetWellKnownGeogCS("WGS84");char*wkt=nullptr;sr.exportToWkt(&wkt);
 auto path=(root/"a/b/c/d/test.tif").string();auto ds=GetGDALDriverManager()->GetDriverByName("GTiff")->Create(path.c_str(),32,32,1,GDT_Int16,nullptr);Check(ds,"create raster");double gt[]={126,1.0/32,0,38,0,-1.0/32};ds->SetGeoTransform(gt);ds->SetProjection(wkt);ds->GetRasterBand(1)->SetNoDataValue(-32767);std::vector<short> a(1024,150);a[0]=0;a[1]=-15;a[2]=-32767;Check(ds->GetRasterBand(1)->RasterIO(GF_Write,0,0,32,32,a.data(),32,32,GDT_Int16,0,0,nullptr)==CE_None,"write samples");GDALClose(ds);
 // Real DTED Level 2 driver/file: 3601 x 3601 posts, one degree cell.
 auto mem=GetGDALDriverManager()->GetDriverByName("MEM")->Create("",3601,3601,1,GDT_Int16,nullptr);double dtgt[]={127-0.5/3600,1.0/3600,0,38+0.5/3600,0,-1.0/3600};mem->SetGeoTransform(dtgt);mem->SetProjection(wkt);mem->GetRasterBand(1)->Fill(321);mem->GetRasterBand(1)->SetNoDataValue(-32767);
 auto dtpath=(root/"a/n37.dt2").string();auto dted=GetGDALDriverManager()->GetDriverByName("DTED")->CreateCopy(dtpath.c_str(),mem,FALSE,nullptr,nullptr,nullptr);Check(dted,"create DTED2");GDALClose(dted);GDALClose(mem);CPLFree(wkt);
 h=TerrainCreate(root.u8string().c_str());Check(h,"create engine");WaitIndex(h);double value;
 Check(WaitSample(h,TerrainRequestSample(h,127.5,37.5),value)==1&&value==321,"DTED2 height");
 Check(WaitSample(h,TerrainRequestSample(h,126.5,37.5),value)==1&&value==150,"height");
 Check(WaitSample(h,TerrainRequestSample(h,126+.5/32,38-.5/32),value)==1&&value==0,"zero is valid");
 Check(WaitSample(h,TerrainRequestSample(h,126+1.5/32,38-.5/32),value)==1&&value==-15,"negative is valid");
 Check(WaitSample(h,TerrainRequestSample(h,126+2.5/32,38-.5/32),value)==2,"nodata");Check(WaitSample(h,TerrainRequestSample(h,20,10),value)==2,"outside coverage");
 Check(TerrainRequestSample(h,0,91)==0,"invalid latitude");
 TerrainView v{126,37,128,38,256,128};auto ticket=TerrainRequestView(h,&v);Check(ticket,"view ticket");std::vector<unsigned char> pixels(256*128*4);int result=0;
 for(int i=0;i<500&&!result;++i){result=TerrainCopyFrame(h,ticket,pixels.data(),pixels.size());if(!result)std::this_thread::sleep_for(std::chrono::milliseconds(20));}Check(result==1,"render ready");Check(pixels[(64*256+64)*4+3]==255&&pixels[(64*256+192)*4+3]==255,"both DEM files rendered");Check(TerrainCopyFrame(h,ticket,pixels.data(),1)==-1,"buffer size checked");
 v.west+=360;v.east+=360;auto next=TerrainRequestView(h,&v);Check(TerrainCopyFrame(h,ticket,pixels.data(),pixels.size())==0,"stale view rejected");result=0;for(int i=0;i<500&&!result;++i){result=TerrainCopyFrame(h,next,pixels.data(),pixels.size());if(!result)std::this_thread::sleep_for(std::chrono::milliseconds(20));}Check(result==1&&pixels[(64*256+192)*4+3]==255,"world wrap rendering");
 // Supersede in-flight work and shut down with pending requests.
 for(int i=0;i<20;++i){v.west=126+i*0.001;v.east=v.west+2;Check(TerrainRequestView(h,&v)!=0,"rapid view request");TerrainRequestSample(h,127+i*0.001,37.5);}
 TerrainDestroy(h);h=nullptr;std::filesystem::remove_all(root);std::cout<<"PASS DTED2, four-level scan, metres, zero/negative/nodata, async frame, stale tickets, world wrap\n";return 0;
 }catch(const std::exception&e){if(h)TerrainDestroy(h);std::cerr<<"FAIL "<<e.what()<<"\n";return 1;}}
