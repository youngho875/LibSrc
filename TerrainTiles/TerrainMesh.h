#pragma once
#include <array>
#include <cstdint>
#include <vector>
namespace tiles
{
struct MeshVertex
{
    double x, y, height;
};
class TerrainMesh
{
public:
    void SetVertices(const std::vector<double>& x, const std::vector<double>& y,
                     const std::vector<double>& height);
    void AddTriangle(uint32_t a, uint32_t b, uint32_t c);
    bool HasMesh() const;
    bool Height(double x, double y, float& height) const;
private:
    static constexpr int Bins = 32;
    std::vector<MeshVertex> vertices;
    std::vector<std::array<uint32_t, 3>> triangles;
    std::array<std::vector<uint32_t>, Bins * Bins> index;
    size_t references = 0;
    bool TriangleHeight(uint32_t triangle, double x, double y, float& height) const;
};
}
