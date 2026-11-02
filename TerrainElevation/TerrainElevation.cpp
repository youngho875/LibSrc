#define TERRAIN_ELEVATION_EXPORTS
#include "TerrainElevation.h"
#include <gdal_priv.h>
#include <gdalwarper.h>
#include <ogr_spatialref.h>
#include <cpl_conv.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
namespace {
struct Close {void operator()(GDALDataset* p)const{if(p)GDALClose(p);}};
using Dataset=std::unique_ptr<GDALDataset,Close>;
struct Raster {std::string path,wkt;double west,south,east,north,resolution,unit;};
struct Request {TerrainView view{};uint64_t ticket=0;};
bool Valid(const TerrainView& v){return v.width>0&&v.height>0&&v.width<=2048&&v.height<=2048&&std::isfinite(v.west)&&std::isfinite(v.east)&&std::isfinite(v.north)&&std::isfinite(v.south)&&v.east>v.west&&v.east-v.west<=720&&v.north>v.south&&v.north<=90&&v.south>=-90&&fabs(v.west)<=1080&&fabs(v.east)<=1080;}
double Wrap(double lon){lon=fmod(lon+180,360);if(lon<0)lon+=360;return lon-180;}
OGRSpatialReference WGS(){OGRSpatialReference s;s.SetWellKnownGeogCS("WGS84");s.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);return s;}
struct Transform {OGRCoordinateTransformation* p=nullptr;~Transform(){if(p)OCTDestroyCoordinateTransformation(p);}};
struct Engine {
    std::mutex mutex;std::condition_variable cv;std::thread worker;std::atomic<bool> stop{false};
    std::atomic<uint64_t> newestView{0};uint64_t serial=0,frameDone=0,sampleDone=0;
    Request wanted;bool viewPending=false,samplePending=false,indexing=true;
    double sampleLon=0,sampleLat=0,sampleHeight=0;uint64_t sampleTicket=0;bool sampleValid=false;
    std::vector<unsigned char> frame;std::vector<Raster> rasters;uint32_t files=0,rejected=0;
    std::string root;
    explicit Engine(const char* path):root(path){worker=std::thread([this]{Run();});}
    ~Engine(){stop=true;cv.notify_all();if(worker.joinable())worker.join();}
    void Index(){
        std::error_code ec;auto directory=std::filesystem::u8path(root);
        auto options=std::filesystem::directory_options::skip_permission_denied;
        for(std::filesystem::recursive_directory_iterator it(directory,options,ec),end;it!=end&&!stop;it.increment(ec)){
            if(ec){ec.clear();continue;}if(!it->is_regular_file(ec))continue;
            auto ext=it->path().extension().u8string();for(auto& c:ext)c=(char)tolower((unsigned char)c);
            if(ext!=".dt2"&&ext!=".dt1"&&ext!=".dt0"&&ext!=".hgt"&&ext!=".tif"&&ext!=".tiff"&&ext!=".dem"&&ext!=".img"&&ext!=".vrt")continue;
            Raster r;r.path=it->path().u8string();bool accepted=false;
            Dataset ds((GDALDataset*)GDALOpenEx(r.path.c_str(),GDAL_OF_RASTER|GDAL_OF_READONLY,nullptr,nullptr,nullptr));
            if(ds&&ds->GetRasterCount()==1){double gt[6];auto srs=ds->GetSpatialRef();
                if(srs&&ds->GetGeoTransform(gt)==CE_None&&ds->GetRasterXSize()>0&&ds->GetRasterYSize()>0){
                    OGRSpatialReference src=*srs;src.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);auto target=WGS();Transform tr;tr.p=OGRCreateCoordinateTransformation(&src,&target);
                    if(tr.p){r.west=r.south=1e100;r.east=r.north=-1e100;bool valid=true;
                        // Densify edges for projected DEM extents.
                        for(int edge=0;edge<4&&valid;++edge)for(int i=0;i<=16;++i){double t=i/16.0,px=(edge==0?0:edge==1?1:t)*ds->GetRasterXSize(),py=(edge==2?0:edge==3?1:t)*ds->GetRasterYSize();double x=gt[0]+px*gt[1]+py*gt[2],y=gt[3]+px*gt[4]+py*gt[5];if(!tr.p->Transform(1,&x,&y)||!std::isfinite(x)||!std::isfinite(y)){valid=false;break;}r.west=std::min(r.west,x);r.east=std::max(r.east,x);r.south=std::min(r.south,y);r.north=std::max(r.north,y);}
                        const std::string unit=ds->GetRasterBand(1)->GetUnitType();r.unit=(unit=="ft"||unit=="foot"||unit=="feet")?0.3048:1;
                        if(valid&&r.east>r.west&&r.north>r.south&&r.east-r.west<=360){char* wkt=nullptr;src.exportToWkt(&wkt);r.wkt=wkt?wkt:"";CPLFree(wkt);r.resolution=(r.east-r.west)/ds->GetRasterXSize();rasters.push_back(std::move(r));accepted=true;}
                    }
                }
            }
            std::lock_guard<std::mutex> lock(mutex);if(accepted)++files;else ++rejected;
        }
        // Coarse first; fine datasets replace coarse pixels in render and sample.
        std::stable_sort(rasters.begin(),rasters.end(),[](const Raster&a,const Raster&b){return a.resolution>b.resolution;});
        std::lock_guard<std::mutex> lock(mutex);indexing=false;
    }
    bool Sample(double lon,double lat,double& height){
        lon=Wrap(lon);
        for(auto it=rasters.rbegin();it!=rasters.rend()&&!stop;++it){auto&r=*it;if(lon<r.west||lon>r.east||lat<r.south||lat>r.north)continue;
            Dataset ds((GDALDataset*)GDALOpenEx(r.path.c_str(),GDAL_OF_RASTER|GDAL_OF_READONLY,nullptr,nullptr,nullptr));if(!ds)continue;
            auto source=WGS();OGRSpatialReference target;target.SetFromUserInput(r.wkt.c_str());target.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);Transform tr;tr.p=OGRCreateCoordinateTransformation(&source,&target);double x=lon,y=lat,gt[6],inv[6];if(!tr.p||!tr.p->Transform(1,&x,&y)||ds->GetGeoTransform(gt)!=CE_None||!GDALInvGeoTransform(gt,inv))continue;
            int px=(int)floor(inv[0]+inv[1]*x+inv[2]*y),py=(int)floor(inv[3]+inv[4]*x+inv[5]*y);if(px<0||py<0||px>=ds->GetRasterXSize()||py>=ds->GetRasterYSize())continue;
            auto band=ds->GetRasterBand(1);double value;int has=0;double nodata=band->GetNoDataValue(&has);
            if(band->RasterIO(GF_Read,px,py,1,1,&value,1,1,GDT_Float64,0,0,nullptr)==CE_None&&std::isfinite(value)&&(!has||value!=nodata)){
                int ok=0;double scale=band->GetScale(&ok);if(!ok)scale=1;double offset=band->GetOffset(&ok);if(!ok)offset=0;height=(value*scale+offset)*r.unit;return std::isfinite(height);
            }
        }return false;
    }
    void ServiceSample(){
        uint64_t ticket=0;double lon=0,lat=0;
        {std::lock_guard<std::mutex> lock(mutex);if(!samplePending)return;ticket=sampleTicket;lon=sampleLon;lat=sampleLat;samplePending=false;}
        double height=0;bool valid=Sample(lon,lat,height);
        std::lock_guard<std::mutex> lock(mutex);if(sampleTicket==ticket){sampleDone=ticket;sampleHeight=height;sampleValid=valid;}
    }
    static int Progress(double,const char*,void* user){auto pair=(std::pair<Engine*,uint64_t>*)user;return !pair->first->stop&&pair->first->newestView==pair->second;}
    std::vector<unsigned char> Render(const Request& req){
        const auto&v=req.view;size_t count=(size_t)v.width*v.height;const float missing=-3.4e38f;
        std::vector<float> heights(count,missing);auto driver=GetGDALDriverManager()->GetDriverByName("MEM");if(!driver)return {};
        auto target=WGS();char* wkt=nullptr;target.exportToWkt(&wkt);std::string dstWkt=wkt?wkt:"";CPLFree(wkt);
        for(auto&r:rasters){ServiceSample();if(stop||newestView!=req.ticket)return {};if(r.north<v.south||r.south>v.north)continue;
            int first=(int)floor((v.west-r.east)/360),last=(int)ceil((v.east-r.west)/360);
            for(int copy=first;copy<=last;++copy){double shift=copy*360.0;if(r.east+shift<v.west||r.west+shift>v.east)continue;
                Dataset src((GDALDataset*)GDALOpenEx(r.path.c_str(),GDAL_OF_RASTER|GDAL_OF_READONLY,nullptr,nullptr,nullptr));if(!src)continue;
                // At overview scales, avoid warping every full-resolution DTED
                // column. RasterIO selects a bounded preview; point queries still
                // use the original resolution and elevation values.
                const int sw=src->GetRasterXSize(),sh=src->GetRasterYSize();
                double factor=std::max((r.east-r.west)/(v.east-v.west)*v.width/sw,(r.north-r.south)/(v.north-v.south)*v.height/sh)*2;
                if(factor<0.5){int pw=std::clamp((int)ceil(sw*factor),std::min(32,sw),sw),ph=std::clamp((int)ceil(sh*factor),std::min(32,sh),sh);
                    std::vector<float> overview((size_t)pw*ph);auto band=src->GetRasterBand(1);
                    if(band->RasterIO(GF_Read,0,0,sw,sh,overview.data(),pw,ph,GDT_Float32,0,0,nullptr)==CE_None){
                        Dataset preview(driver->Create("",pw,ph,1,GDT_Float32,nullptr));double sourceGT[6];
                        if(preview&&src->GetGeoTransform(sourceGT)==CE_None){sourceGT[1]*=(double)sw/pw;sourceGT[4]*=(double)sw/pw;sourceGT[2]*=(double)sh/ph;sourceGT[5]*=(double)sh/ph;preview->SetGeoTransform(sourceGT);preview->SetProjection(r.wkt.c_str());
                            auto destBand=preview->GetRasterBand(1);int has=0;double noData=band->GetNoDataValue(&has);if(has)destBand->SetNoDataValue(noData);double scale=band->GetScale(&has);if(has)destBand->SetScale(scale);double offset=band->GetOffset(&has);if(has)destBand->SetOffset(offset);
                            if(destBand->RasterIO(GF_Write,0,0,pw,ph,overview.data(),pw,ph,GDT_Float32,0,0,nullptr)==CE_None)src=std::move(preview);
                        }
                    }
                }
                Dataset dst(driver->Create("",v.width,v.height,1,GDT_Float32,nullptr));if(!dst)continue;
                double gt[]={v.west-shift,(v.east-v.west)/v.width,0,v.north,0,-(v.north-v.south)/v.height};dst->SetGeoTransform(gt);dst->SetProjection(dstWkt.c_str());dst->GetRasterBand(1)->SetNoDataValue(missing);dst->GetRasterBand(1)->Fill(missing);
                GDALWarpOptions* options=GDALCreateWarpOptions();options->hSrcDS=src.get();options->hDstDS=dst.get();options->nBandCount=1;options->panSrcBands=(int*)CPLMalloc(sizeof(int));options->panDstBands=(int*)CPLMalloc(sizeof(int));options->panSrcBands[0]=options->panDstBands[0]=1;options->eResampleAlg=GRA_Bilinear;
                int has=0;double nodata=src->GetRasterBand(1)->GetNoDataValue(&has);if(has){options->padfSrcNoDataReal=(double*)CPLMalloc(sizeof(double));options->padfSrcNoDataReal[0]=nodata;}
                options->padfDstNoDataReal=(double*)CPLMalloc(sizeof(double));options->padfDstNoDataReal[0]=missing;
                options->pTransformerArg=GDALCreateGenImgProjTransformer(src.get(),r.wkt.c_str(),dst.get(),dstWkt.c_str(),FALSE,0,1);options->pfnTransformer=GDALGenImgProjTransform;
                std::pair<Engine*,uint64_t> progress(this,req.ticket);options->pfnProgress=Progress;options->pProgressArg=&progress;
                if(options->pTransformerArg){GDALWarpOperation operation;
                    if(operation.Initialize(options)==CE_None&&operation.ChunkAndWarpImage(0,0,v.width,v.height)==CE_None){std::vector<float> pixels(count);if(dst->GetRasterBand(1)->RasterIO(GF_Read,0,0,v.width,v.height,pixels.data(),v.width,v.height,GDT_Float32,0,0,nullptr)==CE_None){int ok=0;double scale=src->GetRasterBand(1)->GetScale(&ok);if(!ok)scale=1;double offset=src->GetRasterBand(1)->GetOffset(&ok);if(!ok)offset=0;for(size_t i=0;i<count;++i)if(pixels[i]!=missing&&std::isfinite(pixels[i]))heights[i]=(float)((pixels[i]*scale+offset)*r.unit);}}
                    GDALDestroyGenImgProjTransformer(options->pTransformerArg);
                }GDALDestroyWarpOptions(options);
            }
        }
        if(stop||newestView!=req.ticket)return {};
        std::vector<unsigned char> out(count*4,0);
        double dx=std::max(1.0,(v.east-v.west)/v.width*111320*cos((v.north+v.south)/2*3.141592653589793/180)),dy=std::max(1.0,(v.north-v.south)/v.height*111320);
        for(int y=0;y<v.height;++y)for(int x=0;x<v.width;++x){size_t i=(size_t)y*v.width+x;float z=heights[i];if(z==missing)continue;
            auto at=[&](int a,int b){float h=heights[(size_t)std::clamp(b,0,v.height-1)*v.width+std::clamp(a,0,v.width-1)];return h==missing?z:h;};
            double gx=(at(x+1,y)-at(x-1,y))/(2*dx),gy=(at(x,y+1)-at(x,y-1))/(2*dy),light=std::clamp((0.6+0.4*(0.5*gx+0.5*gy+0.7071)/sqrt(gx*gx+gy*gy+1)),0.25,1.0);
            double t=std::clamp((double)z/3000,0.0,1.0);out[i*4]=(unsigned char)((105+100*t)*light);out[i*4+1]=(unsigned char)((175+40*t)*light);out[i*4+2]=(unsigned char)((110+115*t)*light);out[i*4+3]=255;
        }return out;
    }
    void Run(){try{static std::once_flag init;std::call_once(init,[]{GDALAllRegister();});Index();while(!stop){Request req;uint64_t sample=0;double lon=0,lat=0;
        {std::unique_lock<std::mutex> lock(mutex);cv.wait(lock,[&]{return stop||samplePending||viewPending;});if(stop)break;if(samplePending){sample=sampleTicket;lon=sampleLon;lat=sampleLat;samplePending=false;}else{req=wanted;viewPending=false;}}
        if(sample){double height=0;bool valid=Sample(lon,lat,height);std::lock_guard<std::mutex> lock(mutex);if(sampleTicket==sample){sampleDone=sample;sampleHeight=height;sampleValid=valid;}}
        else{auto image=Render(req);std::lock_guard<std::mutex> lock(mutex);if(wanted.ticket==req.ticket&&!image.empty()){frame=std::move(image);frameDone=req.ticket;}}
    }}catch(...){std::lock_guard<std::mutex> lock(mutex);indexing=false;++rejected;}}
};
}
extern "C" {
TerrainHandle TerrainCreate(const char* root){try{if(!root||!*root)return nullptr;return new Engine(root);}catch(...){return nullptr;}}
void TerrainDestroy(TerrainHandle h){delete static_cast<Engine*>(h);}
int TerrainGetStatus(TerrainHandle h,TerrainStatus*s){if(!h||!s||s->size!=sizeof(TerrainStatus))return 0;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);s->indexing=e.indexing;s->files=e.files;s->rejected=e.rejected;s->frameRevision=e.frameDone;s->sampleRevision=e.sampleDone;return 1;}
uint64_t TerrainRequestView(TerrainHandle h,const TerrainView*v){if(!h||!v||!Valid(*v))return 0;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);auto&a=e.wanted.view;if(e.wanted.ticket&&a.west==v->west&&a.south==v->south&&a.east==v->east&&a.north==v->north&&a.width==v->width&&a.height==v->height)return e.wanted.ticket;e.wanted={*v,++e.serial};e.newestView=e.wanted.ticket;e.viewPending=true;e.cv.notify_one();return e.wanted.ticket;}
int TerrainCopyFrame(TerrainHandle h,uint64_t ticket,unsigned char*out,size_t bytes){if(!h||!out||!ticket)return -1;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);if(e.frameDone!=ticket||e.wanted.ticket!=ticket)return 0;if(bytes<e.frame.size())return -1;std::copy(e.frame.begin(),e.frame.end(),out);return 1;}
uint64_t TerrainRequestSample(TerrainHandle h,double lon,double lat){if(!h||!std::isfinite(lon)||!std::isfinite(lat)||lat< -90||lat>90)return 0;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);if(e.sampleTicket&&e.sampleLon==lon&&e.sampleLat==lat)return e.sampleTicket;e.sampleLon=lon;e.sampleLat=lat;e.sampleTicket=++e.serial;e.samplePending=true;e.cv.notify_one();return e.sampleTicket;}
int TerrainGetSample(TerrainHandle h,uint64_t ticket,double*height){if(!h||!height||!ticket)return -1;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);if(ticket!=e.sampleTicket||ticket!=e.sampleDone)return 0;if(!e.sampleValid)return 2;*height=e.sampleHeight;return 1;}
}
