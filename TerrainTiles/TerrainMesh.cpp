#include "TerrainMesh.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace tiles
{
void TerrainMesh::SetVertices(const std::vector<double>& x, const std::vector<double>& y,
                              const std::vector<double>& height)
{
    if (x.size() != y.size() || x.size() != height.size())
        throw std::runtime_error("Invalid terrain mesh vertices");
    vertices.reserve(x.size());
    for (size_t i = 0; i < x.size(); ++i)
        vertices.push_back({x[i], y[i], height[i]});
}
void TerrainMesh::AddTriangle(uint32_t a, uint32_t b, uint32_t c)
{
    if (a >= vertices.size() || b >= vertices.size() || c >= vertices.size())
        throw std::runtime_error("Invalid terrain mesh index");
    const auto& p = vertices[a];
    const auto& q = vertices[b];
    const auto& r = vertices[c];
    int x0 = std::clamp(static_cast<int>(floor(std::min({p.x, q.x, r.x}) / 4)), 0, Bins - 1);
    int x1 = std::clamp(static_cast<int>(floor(std::max({p.x, q.x, r.x}) / 4)), 0, Bins - 1);
    int y0 = std::clamp(static_cast<int>(floor(std::min({p.y, q.y, r.y}) / 4)), 0, Bins - 1);
    int y1 = std::clamp(static_cast<int>(floor(std::max({p.y, q.y, r.y}) / 4)), 0, Bins - 1);
    references += static_cast<size_t>(x1 - x0 + 1) * (y1 - y0 + 1);
    if (references > 8000000)
        throw std::runtime_error("Terrain mesh spatial index limit");
    uint32_t id = static_cast<uint32_t>(triangles.size());
    triangles.push_back({a, b, c});
    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x)
            index[y * Bins + x].push_back(id);
}
bool TerrainMesh::HasMesh() const
{
    return !triangles.empty();
}
bool TerrainMesh::TriangleHeight(uint32_t triangle, double x, double y, float& height) const
{
    const auto& ids = triangles[triangle];
    const auto& a = vertices[ids[0]];
    const auto& b = vertices[ids[1]];
    const auto& c = vertices[ids[2]];
    double det = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
    if (fabs(det) < 1e-12)
        return false;
    double wa = ((b.y - c.y) * (x - c.x) + (c.x - b.x) * (y - c.y)) / det;
    double wb = ((c.y - a.y) * (x - c.x) + (a.x - c.x) * (y - c.y)) / det;
    double wc = 1 - wa - wb;
    if (wa < -1e-8 || wb < -1e-8 || wc < -1e-8)
        return false;
    height = static_cast<float>(wa * a.height + wb * b.height + wc * c.height);
    return std::isfinite(height);
}
bool TerrainMesh::Height(double x, double y, float& height) const
{
    if (!std::isfinite(x) || !std::isfinite(y) || x < 0 || y < 0 || x > 128 || y > 128)
        return false;
    int ix = std::clamp(static_cast<int>(x / 4), 0, Bins - 1);
    int iy = std::clamp(static_cast<int>(y / 4), 0, Bins - 1);
    for (uint32_t triangle : index[iy * Bins + ix])
        if (TriangleHeight(triangle, x, y, height))
            return true;
    return false;
}
}
