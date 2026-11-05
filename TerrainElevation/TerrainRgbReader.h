#pragma once
#include <fstream>
// Only mapMake's explicitly marked XYZ folders are interpreted as elevation.
// Decode before interpolation: interpolating encoded RGB channels corrupts heights.
Dataset OpenElevation(const std::string& path,bool pixels=true)
{
    namespace fs=std::filesystem;
    auto file=fs::u8path(path);
    auto ext=file.extension().u8string();
    for(auto& c:ext)c=(char)tolower((unsigned char)c);
    Dataset raw((GDALDataset*)GDALOpenEx(path.c_str(),GDAL_OF_RASTER|GDAL_OF_READONLY,nullptr,nullptr,nullptr));
    if(ext!=".png"||!raw)return raw;
    auto xdir=file.parent_path(),zdir=xdir.parent_path(),root=zdir.parent_path();
    std::ifstream marker(root/".terrain-rgb");std::string tag;std::getline(marker,tag);
    if(tag!="TerrainRGB/1")return {};
    auto number=[](const std::string& s)->int {
        if(s.empty()||s.find_first_not_of("0123456789")!=std::string::npos)return -1;
        try{return std::stoi(s);}catch(...){return -1;}
    };
    int z=number(zdir.filename().u8string()),x=number(xdir.filename().u8string()),y=number(file.stem().u8string());
    if(z<0||z>24||x<0||y<0||x>=(1<<z)||y>=(1<<z)||raw->GetRasterXSize()!=256||raw->GetRasterYSize()!=256||raw->GetRasterCount()!=4)return {};
    auto driver=GetGDALDriverManager()->GetDriverByName("MEM");
    Dataset out(driver?driver->Create("",256,256,1,GDT_Float64,nullptr):nullptr);if(!out)return {};
    const double world=20037508.342789244,span=2*world/(1<<z),missing=-1e30;
    double gt[]={-world+x*span,span/256,0,world-y*span,0,-span/256};
    out->SetGeoTransform(gt);OGRSpatialReference srs;srs.importFromEPSG(3857);out->SetSpatialRef(&srs);
    out->GetRasterBand(1)->SetUnitType("m");out->GetRasterBand(1)->SetNoDataValue(missing);
    if(pixels){
        std::vector<unsigned char> rgba(256*256*4);
        int bands[]={1,2,3,4};
        if(raw->RasterIO(GF_Read,0,0,256,256,rgba.data(),256,256,GDT_Byte,4,bands,4,1024,1,nullptr)!=CE_None)return {};
        std::vector<double> heights(256*256,missing);
        for(size_t i=0;i<heights.size();++i)if(rgba[i*4+3])
            heights[i]=-10000.0+(rgba[i*4]*65536u+rgba[i*4+1]*256u+rgba[i*4+2])*0.1;
        if(out->GetRasterBand(1)->RasterIO(GF_Write,0,0,256,256,heights.data(),256,256,GDT_Float64,0,0,nullptr)!=CE_None)return {};
    }
    return out;
}
