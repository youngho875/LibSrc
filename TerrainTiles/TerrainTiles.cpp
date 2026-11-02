#define TERRAIN_TILES_EXPORTS
#include "TerrainTiles.h"
#include <gdal_priv.h>
#include "TileSource.h"
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
struct Request {TerrainTilesView view{};uint64_t ticket=0;};
bool Valid(const TerrainTilesView& v){return v.width>0&&v.height>0&&v.width<=2048&&v.height<=2048&&std::isfinite(v.west)&&std::isfinite(v.east)&&std::isfinite(v.north)&&std::isfinite(v.south)&&v.east>v.west&&v.east-v.west<=720&&v.north>v.south&&v.north<=90&&v.south>=-90&&fabs(v.west)<=1080&&fabs(v.east)<=1080;}
double Wrap(double lon){lon=fmod(lon+180,360);if(lon<0)lon+=360;return lon-180;}
struct Engine {
    std::mutex mutex;std::condition_variable cv;std::thread worker;std::atomic<bool> stop{false};
    std::atomic<uint64_t> newestView{0};uint64_t serial=0,frameDone=0,frameRevision=0,sampleDone=0;
    Request wanted;bool viewPending=false,samplePending=false,indexing=true;
    double sampleLon=0,sampleLat=0,sampleHeight=0;uint64_t sampleTicket=0;bool sampleValid=false;
    std::vector<unsigned char> frame;uint32_t files=0,rejected=0;
    std::string root,lastError;bool server=false;tiles::Source source;int sampleZoom=0;
    explicit Engine(const char* path,bool remote):root(path),server(remote),source(stop){worker=std::thread([this]{Run();});}
    ~Engine(){stop=true;cv.notify_all();if(worker.joinable())worker.join();}
    void Index(){try{source.Open(root,server);sampleZoom=source.minzoom;std::lock_guard<std::mutex> lock(mutex);files=1;indexing=false;}catch(const std::exception&e){std::lock_guard<std::mutex> lock(mutex);lastError=e.what();indexing=false;++rejected;}}
    bool Sample(double lon,double lat,double&height){if(!source.ready)return false;source.budget=4;source.deadline=std::chrono::steady_clock::now()+std::chrono::seconds(3);float h=0;bool ok=source.Sample(lon,lat,sampleZoom,h);height=h;return ok;}
    void ServiceSample(){
        uint64_t ticket=0;double lon=0,lat=0;
        {std::lock_guard<std::mutex> lock(mutex);if(!samplePending)return;ticket=sampleTicket;lon=sampleLon;lat=sampleLat;samplePending=false;}
        double height=0;bool valid=Sample(lon,lat,height);
        std::lock_guard<std::mutex> lock(mutex);if(sampleTicket==ticket){sampleDone=ticket;sampleHeight=height;sampleValid=valid;}
    }
    std::vector<unsigned char> Render(const Request& req){
        const auto&v=req.view;size_t count=(size_t)v.width*v.height;const float missing=tiles::missing;
        std::vector<float> heights(count,missing);if(!source.ready)return std::vector<unsigned char>(count*4,0);
        int zoom=std::clamp((int)floor(log2((source.mercator?360.:180.)*v.width/(128*(v.east-v.west)))),source.minzoom,source.maxzoom);
        while(zoom>source.minzoom){double nx=(source.mercator?1:2)*(1<<zoom),ny=1<<zoom;
            if((ceil((v.east-v.west)/360*nx)+1)*(ceil((source.Y(v.north)-source.Y(v.south))*ny)+1)<=64)break;--zoom;}
        double columns=ceil((v.east-v.west)/360*(source.mercator?1:2)*(1<<zoom))+1;
        double rows=ceil((source.Y(v.north)-source.Y(v.south))*(1<<zoom))+1;
        source.incomplete=false;
        if(columns*rows>64){std::lock_guard<std::mutex>lock(mutex);lastError="Terrain minimum zoom requires a closer view";return std::vector<unsigned char>(count*4,0);}
        if(sampleZoom!=zoom){std::lock_guard<std::mutex>lock(mutex);if(sampleTicket){samplePending=true;sampleTicket=++serial;}sampleZoom=zoom;}
        source.budget=96;source.deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);
        if(source.ready)for(int y=0;y<v.height;++y){if(stop||newestView!=req.ticket)return {};if(y%32==0){int remaining=source.budget;auto end=source.deadline;bool incomplete=source.incomplete;ServiceSample();source.incomplete=incomplete;source.budget=remaining;source.deadline=end;}
            double lat=v.north-(y+.5)*(v.north-v.south)/v.height;
            for(int x=0;x<v.width;++x){float h;if(source.Sample(v.west+(x+.5)*(v.east-v.west)/v.width,lat,zoom,h))heights[(size_t)y*v.width+x]=h;}
        }
        {std::lock_guard<std::mutex>lock(mutex);rejected=(uint32_t)source.failures;lastError=source.error;}
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
        else{auto image=Render(req);std::lock_guard<std::mutex> lock(mutex);if(wanted.ticket==req.ticket&&!image.empty()){frame=std::move(image);frameDone=req.ticket;++frameRevision;if(source.incomplete)viewPending=true;}}
    }}catch(...){std::lock_guard<std::mutex> lock(mutex);indexing=false;++rejected;}}
};
}
extern "C" {
TerrainTilesHandle TerrainTilesCreate(const char* root,int server){try{if(!root||!*root)return nullptr;if(server!=0&&server!=1)return nullptr;return new Engine(root,server!=0);}catch(...){return nullptr;}}
void TerrainTilesDestroy(TerrainTilesHandle h){delete static_cast<Engine*>(h);}
int TerrainTilesGetStatus(TerrainTilesHandle h,TerrainTilesStatus*s){if(!h||!s||s->size!=sizeof(TerrainTilesStatus))return 0;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);s->indexing=e.indexing;s->files=e.files;s->rejected=e.rejected;s->frameRevision=e.frameRevision;s->sampleRevision=e.sampleDone;return 1;}
uint64_t TerrainTilesRequestView(TerrainTilesHandle h,const TerrainTilesView*v){if(!h||!v||!Valid(*v))return 0;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);auto&a=e.wanted.view;if(e.wanted.ticket&&a.west==v->west&&a.south==v->south&&a.east==v->east&&a.north==v->north&&a.width==v->width&&a.height==v->height)return e.wanted.ticket;e.wanted={*v,++e.serial};e.newestView=e.wanted.ticket;e.viewPending=true;e.cv.notify_one();return e.wanted.ticket;}
int TerrainTilesCopyFrame(TerrainTilesHandle h,uint64_t ticket,unsigned char*out,size_t bytes){if(!h||!out||!ticket)return -1;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);if(e.frameDone!=ticket||e.wanted.ticket!=ticket)return 0;if(bytes<e.frame.size())return -1;std::copy(e.frame.begin(),e.frame.end(),out);return 1;}
uint64_t TerrainTilesRequestSample(TerrainTilesHandle h,double lon,double lat){if(!h||!std::isfinite(lon)||!std::isfinite(lat)||lat< -90||lat>90)return 0;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);if(e.sampleTicket&&e.sampleLon==lon&&e.sampleLat==lat)return e.sampleTicket;e.sampleLon=lon;e.sampleLat=lat;e.sampleTicket=++e.serial;e.samplePending=true;e.cv.notify_one();return e.sampleTicket;}
int TerrainTilesGetSample(TerrainTilesHandle h,uint64_t ticket,double*height){if(!h||!height||!ticket)return -1;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);if(ticket!=e.sampleTicket||ticket!=e.sampleDone)return 0;if(!e.sampleValid)return 2;*height=e.sampleHeight;return 1;}
int TerrainTilesGetError(TerrainTilesHandle h,char*out,size_t bytes){if(!h||!out||!bytes)return 0;auto&e=*(Engine*)h;std::lock_guard<std::mutex> lock(e.mutex);size_t n=std::min(bytes-1,e.lastError.size());memcpy(out,e.lastError.data(),n);out[n]=0;return (int)n;}

}
