#pragma once
#include "ModelAssets.h"
#include <vector>
#include <string>
#include <filesystem>

namespace modelassets
{
struct Mesh
{
    std::vector<MA_Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t material = 0;
};
struct Texture
{
    std::vector<unsigned char> rgba;
    uint32_t width = 0, height = 0;
};
struct Asset
{
    MA_Info info{};
    std::vector<Mesh> meshes;
    std::vector<MA_Material> materials;
    std::vector<Texture> textures;
    std::string batch;
};
struct B3dm
{
    std::vector<unsigned char> glb;
    double center[3]{};
    std::string batch;
};
std::vector<unsigned char> ReadBytes(const std::filesystem::path &path);
B3dm ReadB3dm(const std::vector<unsigned char> &bytes);
Texture DecodeImage(const unsigned char *bytes, size_t size);
} // namespace modelassets
