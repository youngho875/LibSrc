#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef MODEL_ASSETS_EXPORTS
#define MA_API __declspec(dllexport)
#else
#define MA_API __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#pragma pack(push, 8)
    typedef void *MA_Handle;
    typedef struct MA_Vertex
    {
        float position[3];
        float normal[3];
        float uv[2];
        float color[4];
    } MA_Vertex;
    typedef struct MA_Mesh
    {
        const MA_Vertex *vertices;
        const uint32_t *indices;
        uint32_t vertexCount, indexCount, material;
    } MA_Mesh;
    typedef struct MA_Material
    {
        float baseColor[4];
        int32_t texture;
        uint32_t doubleSided;
        uint32_t alphaMode; /* 0=opaque, 1=mask, 2=blend */
        float alphaCutoff;
    } MA_Material;
    typedef struct MA_Texture
    {
        const unsigned char *rgba; /* Bottom-up RGBA8 for OpenGL. */
        uint32_t width, height;
    } MA_Texture;
    typedef struct MA_Info
    {
        uint32_t meshCount, materialCount, textureCount, isB3dm;
        double minimum[3], maximum[3]; /* Assimp's right-handed Y-up, metres unless asset says otherwise. */
        double rtcCenter[3];           /* b3dm RTC_CENTER, in tile coordinates, applied AFTER Y-up to Z-up. */
        uint64_t bytes;
    } MA_Info;
#pragma pack(pop)
    /* UTF-8 paths. Geometry pointers remain valid until MA_Destroy; no CRT ownership
       crosses this boundary. Concurrent loads are independent. Static model data. */
    MA_API uint32_t MA_Version(void);
    MA_API int MA_Load(const char *path, MA_Handle *out, char *error, size_t errorBytes);
    MA_API void MA_Destroy(MA_Handle handle);
    MA_API int MA_GetInfo(MA_Handle handle, MA_Info *info);
    MA_API int MA_GetMesh(MA_Handle handle, uint32_t index, MA_Mesh *mesh);
    MA_API int MA_GetMaterial(MA_Handle handle, uint32_t index, MA_Material *material);
    MA_API int MA_GetTexture(MA_Handle handle, uint32_t index, MA_Texture *texture);
    MA_API const char *MA_BatchTable(MA_Handle handle);
#ifdef __cplusplus
}
#endif
