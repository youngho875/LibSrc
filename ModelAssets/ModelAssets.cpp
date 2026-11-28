#include "ModelAssetsInternal.h"
#include <assimp/Importer.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/GltfMaterial.h>
#include <algorithm>
#include <memory>
#include <map>
#include <limits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace
{
class WideStream final : public Assimp::IOStream
{
  public:
    explicit WideStream(FILE *file) : file_(file)
    {
    }
    ~WideStream() override
    {
        fclose(file_);
    }
    size_t Read(void *out, size_t size, size_t count) override
    {
        return fread(out, size, count, file_);
    }
    size_t Write(const void *, size_t, size_t) override
    {
        return 0;
    }
    aiReturn Seek(size_t offset, aiOrigin origin) override
    {
        return _fseeki64(file_, static_cast<__int64>(offset), static_cast<int>(origin)) == 0
                   ? aiReturn_SUCCESS
                   : aiReturn_FAILURE;
    }
    size_t Tell() const override
    {
        return static_cast<size_t>(_ftelli64(file_));
    }
    size_t FileSize() const override
    {
        auto pos = _ftelli64(file_);
        _fseeki64(file_, 0, SEEK_END);
        auto size = _ftelli64(file_);
        _fseeki64(file_, pos, SEEK_SET);
        return static_cast<size_t>(size);
    }
    void Flush() override
    {
    }

  private:
    FILE *file_;
};
class WideIO final : public Assimp::IOSystem
{
  public:
    explicit WideIO(std::filesystem::path root) : root_(std::move(root))
    {
    }
    bool Exists(const char *path) const override
    {
        std::error_code error;
        return std::filesystem::is_regular_file(Resolve(path), error);
    }
    char getOsSeparator() const override
    {
        return '/';
    }
    Assimp::IOStream *Open(const char *path, const char *mode = "rb") override
    {
        if (strchr(mode, 'w') || strchr(mode, '+'))
            return nullptr;
        FILE *file = nullptr;
        _wfopen_s(&file, Resolve(path).c_str(), L"rb");
        return file ? new WideStream(file) : nullptr;
    }
    void Close(Assimp::IOStream *stream) override
    {
        delete stream;
    }

  private:
    std::filesystem::path root_;
    std::filesystem::path Resolve(const char *name) const
    {
        auto path = std::filesystem::u8path(name);
        return path.is_absolute() ? path : root_ / path;
    }
};
using modelassets::Asset;
void Budget(Asset &asset, uint64_t extra)
{
    asset.info.bytes += extra;
    if (asset.info.bytes > 512ull * 1024 * 1024)
        throw std::runtime_error("Decoded asset exceeds 512 MiB");
}
std::unique_ptr<Asset> Load(const std::string &name)
{
    auto path = std::filesystem::absolute(std::filesystem::u8path(name));
    std::string ext = path.extension().u8string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return char(tolower(c)); });
    if (ext != ".3ds" && ext != ".gltf" && ext != ".glb" && ext != ".b3dm")
        throw std::runtime_error("Supported models: .3ds, .gltf, .glb, .b3dm");
    auto asset = std::make_unique<Asset>();
    Assimp::Importer importer;
    importer.SetIOHandler(new WideIO(path.parent_path()));
    constexpr unsigned flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                               aiProcess_GenSmoothNormals | aiProcess_SortByPType |
                               aiProcess_ImproveCacheLocality | aiProcess_PreTransformVertices |
                               aiProcess_ValidateDataStructure;
    const aiScene *scene = nullptr;
    modelassets::B3dm tile;
    if (ext == ".b3dm")
    {
        tile = modelassets::ReadB3dm(modelassets::ReadBytes(path));
        asset->info.isB3dm = 1;
        std::copy(tile.center, tile.center + 3, asset->info.rtcCenter);
        asset->batch = tile.batch;
        scene = importer.ReadFileFromMemory(tile.glb.data(), tile.glb.size(), flags, "glb");
    }
    else
        scene = importer.ReadFile(path.u8string(), flags);
    if (!scene || !scene->HasMeshes())
        throw std::runtime_error(std::string("Assimp: ") + importer.GetErrorString());
    if (scene->mNumMeshes > 100000 || scene->mNumMaterials > 100000)
        throw std::runtime_error("Too many mesh/material records");
    std::map<std::string, int> textures;
    for (unsigned i = 0; i < scene->mNumMaterials; ++i)
    {
        auto source = scene->mMaterials[i];
        MA_Material material{};
        material.texture = -1;
        material.alphaCutoff = .5f;
        aiColor4D color(1, 1, 1, 1);
        if (source->Get(AI_MATKEY_BASE_COLOR, color) != AI_SUCCESS)
            source->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material.baseColor[0] = color.r;
        material.baseColor[1] = color.g;
        material.baseColor[2] = color.b;
        material.baseColor[3] = color.a;
        int sided = 0;
        source->Get(AI_MATKEY_TWOSIDED, sided);
        material.doubleSided = sided != 0;
        aiString alpha;
        source->Get(AI_MATKEY_GLTF_ALPHAMODE, alpha);
        if (std::string(alpha.C_Str()) == "MASK")
            material.alphaMode = 1;
        if (std::string(alpha.C_Str()) == "BLEND" || color.a < .999f)
            material.alphaMode = 2;
        source->Get(AI_MATKEY_GLTF_ALPHACUTOFF, material.alphaCutoff);
        aiString texture;
        if (source->GetTexture(aiTextureType_BASE_COLOR, 0, &texture) == AI_SUCCESS ||
            source->GetTexture(aiTextureType_DIFFUSE, 0, &texture) == AI_SUCCESS)
        {
            std::string key = texture.C_Str();
            auto found = textures.find(key);
            if (found != textures.end())
                material.texture = found->second;
            else
            {
                modelassets::Texture decoded;
                const aiTexture *embedded = scene->GetEmbeddedTexture(key.c_str());
                if (embedded && !embedded->mHeight)
                    decoded = modelassets::DecodeImage(
                        reinterpret_cast<const unsigned char *>(embedded->pcData), embedded->mWidth);
                else if (embedded)
                {
                    if (embedded->mWidth > 8192 || embedded->mHeight > 8192)
                        throw std::runtime_error("Embedded texture dimensions too large");
                    decoded.width = embedded->mWidth;
                    decoded.height = embedded->mHeight;
                    decoded.rgba.resize(size_t(decoded.width) * decoded.height * 4);
                    for (unsigned y = 0; y < decoded.height; ++y)
                        for (unsigned x = 0; x < decoded.width; ++x)
                        {
                            const auto &pixel =
                                embedded->pcData[(decoded.height - 1 - y) * decoded.width + x];
                            auto dest = decoded.rgba.data() + (size_t(y) * decoded.width + x) * 4;
                            dest[0] = pixel.r;
                            dest[1] = pixel.g;
                            dest[2] = pixel.b;
                            dest[3] = pixel.a;
                        }
                }
                else
                {
                    auto bytes = modelassets::ReadBytes(path.parent_path() / std::filesystem::u8path(key));
                    decoded = modelassets::DecodeImage(bytes.data(), bytes.size());
                }
                Budget(*asset, decoded.rgba.size());
                material.texture = static_cast<int>(asset->textures.size());
                textures[key] = material.texture;
                asset->textures.push_back(std::move(decoded));
            }
        }
        asset->materials.push_back(material);
    }
    for (int j = 0; j < 3; ++j)
    {
        asset->info.minimum[j] = std::numeric_limits<double>::infinity();
        asset->info.maximum[j] = -asset->info.minimum[j];
    }
    for (unsigned i = 0; i < scene->mNumMeshes; ++i)
    {
        const auto mesh = scene->mMeshes[i];
        if (!mesh->HasPositions() || !(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
            continue;
        if (mesh->mNumVertices > 10000000 || mesh->mNumFaces > 10000000)
            throw std::runtime_error("Mesh exceeds vertex/triangle limit");
        modelassets::Mesh out;
        out.material = mesh->mMaterialIndex;
        out.vertices.resize(mesh->mNumVertices);
        Budget(*asset, out.vertices.size() * sizeof(MA_Vertex));
        for (unsigned v = 0; v < mesh->mNumVertices; ++v)
        {
            auto &dst = out.vertices[v];
            auto pos = mesh->mVertices[v];
            auto normal = mesh->HasNormals() ? mesh->mNormals[v] : aiVector3D(0, 1, 0);
            for (int j = 0; j < 3; ++j)
            {
                if (!std::isfinite(pos[j]))
                    throw std::runtime_error("Nonfinite mesh position");
                dst.position[j] = pos[j];
                dst.normal[j] = std::isfinite(normal[j]) ? normal[j] : 0;
                asset->info.minimum[j] = std::min(asset->info.minimum[j], double(pos[j]));
                asset->info.maximum[j] = std::max(asset->info.maximum[j], double(pos[j]));
            }
            if (mesh->HasTextureCoords(0))
            {
                dst.uv[0] = mesh->mTextureCoords[0][v].x;
                dst.uv[1] = mesh->mTextureCoords[0][v].y;
            }
            auto color = mesh->HasVertexColors(0) ? mesh->mColors[0][v] : aiColor4D(1, 1, 1, 1);
            dst.color[0] = color.r;
            dst.color[1] = color.g;
            dst.color[2] = color.b;
            dst.color[3] = color.a;
        }
        for (unsigned f = 0; f < mesh->mNumFaces; ++f)
        {
            auto &face = mesh->mFaces[f];
            if (face.mNumIndices != 3)
                continue;
            for (unsigned n = 0; n < 3; ++n)
            {
                if (face.mIndices[n] >= mesh->mNumVertices)
                    throw std::runtime_error("Invalid face index");
                out.indices.push_back(face.mIndices[n]);
            }
        }
        Budget(*asset, out.indices.size() * 4);
        if (!out.indices.empty())
            asset->meshes.push_back(std::move(out));
    }
    if (asset->meshes.empty())
        throw std::runtime_error("No triangle meshes in model");
    asset->info.meshCount = static_cast<uint32_t>(asset->meshes.size());
    asset->info.materialCount = static_cast<uint32_t>(asset->materials.size());
    asset->info.textureCount = static_cast<uint32_t>(asset->textures.size());
    return asset;
}
} // namespace
uint32_t MA_Version()
{
    return 1;
}
int MA_Load(const char *path, MA_Handle *out, char *error, size_t bytes)
{
    if (out)
        *out = nullptr;
    if (error && bytes)
        error[0] = 0;
    try
    {
        if (!path || !out)
            throw std::runtime_error("Invalid argument");
        *out = Load(path).release();
        return 1;
    }
    catch (const std::exception &e)
    {
        if (error && bytes)
            strncpy_s(error, bytes, e.what(), _TRUNCATE);
    }
    catch (...)
    {
        if (error && bytes)
            strncpy_s(error, bytes, "Unexpected model load failure", _TRUNCATE);
    }
    return 0;
}
void MA_Destroy(MA_Handle h)
{
    delete static_cast<Asset *>(h);
}
int MA_GetInfo(MA_Handle h, MA_Info *out)
{
    if (!h || !out)
        return 0;
    *out = static_cast<Asset *>(h)->info;
    return 1;
}
int MA_GetMesh(MA_Handle h, uint32_t i, MA_Mesh *out)
{
    if (!h || !out)
        return 0;
    auto &a = *static_cast<Asset *>(h);
    if (i >= a.meshes.size())
        return 0;
    auto &m = a.meshes[i];
    *out = {m.vertices.data(), m.indices.data(), static_cast<uint32_t>(m.vertices.size()),
            static_cast<uint32_t>(m.indices.size()), m.material};
    return 1;
}
int MA_GetMaterial(MA_Handle h, uint32_t i, MA_Material *out)
{
    if (!h || !out)
        return 0;
    auto &a = *static_cast<Asset *>(h);
    if (i >= a.materials.size())
        return 0;
    *out = a.materials[i];
    return 1;
}
int MA_GetTexture(MA_Handle h, uint32_t i, MA_Texture *out)
{
    if (!h || !out)
        return 0;
    auto &a = *static_cast<Asset *>(h);
    if (i >= a.textures.size())
        return 0;
    auto &t = a.textures[i];
    *out = {t.rgba.data(), t.width, t.height};
    return 1;
}
const char *MA_BatchTable(MA_Handle h)
{
    return h ? static_cast<Asset *>(h)->batch.c_str() : "";
}
