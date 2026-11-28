#include "ModelAssetsInternal.h"
#include <cpl_json.h>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cmath>
namespace modelassets
{
std::vector<unsigned char> ReadBytes(const std::filesystem::path &path)
{
    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    if (!stream)
        throw std::runtime_error("Cannot open model/resource: " + path.u8string());
    auto length = stream.tellg();
    if (length <= 0 || length > 512ll * 1024 * 1024)
        throw std::runtime_error("Resource is empty or exceeds 512 MiB");
    std::vector<unsigned char> data(static_cast<size_t>(length));
    stream.seekg(0);
    if (!stream.read(reinterpret_cast<char *>(data.data()), length))
        throw std::runtime_error("Truncated model/resource");
    return data;
}
B3dm ReadB3dm(const std::vector<unsigned char> &bytes)
{
    auto word = [&](size_t offset) {
        uint32_t n;
        std::memcpy(&n, bytes.data() + offset, 4);
        return n;
    };
    if (bytes.size() < 28 || std::memcmp(bytes.data(), "b3dm", 4) != 0 || word(4) != 1 ||
        word(8) != bytes.size())
        throw std::runtime_error("Invalid b3dm v1 header (28-byte header required)");
    const uint32_t fj = word(12), fb = word(16), bj = word(20), bb = word(24);
    const uint64_t offset = 28ull + fj + fb + bj + bb;
    if (offset + 12 > bytes.size())
        throw std::runtime_error("b3dm table lengths exceed file bounds");
    B3dm result;
    if (fj)
    {
        CPLJSONDocument json;
        if (!json.LoadMemory(std::string(reinterpret_cast<const char *>(bytes.data() + 28), fj)))
            throw std::runtime_error("Invalid b3dm feature-table JSON");
        auto root = json.GetRoot();
        auto rtc = root.GetArray("RTC_CENTER");
        if (rtc.Size() == 3)
            for (int i = 0; i < 3; ++i)
                result.center[i] = rtc[i].ToDouble();
        else if (root["RTC_CENTER"].IsValid())
        {
            const auto pos = root["RTC_CENTER"].GetLong("byteOffset", -1);
            if (pos < 0 || uint64_t(pos) + 12 > fb)
                throw std::runtime_error("Invalid binary RTC_CENTER range");
            float center[3];
            std::memcpy(center, bytes.data() + 28 + fj + pos, 12);
            for (int i = 0; i < 3; ++i)
                result.center[i] = center[i];
        }
        for (double v : result.center)
            if (!std::isfinite(v))
                throw std::runtime_error("Nonfinite RTC_CENTER");
    }
    if (bj)
        result.batch.assign(reinterpret_cast<const char *>(bytes.data() + 28ull + fj + fb), bj);
    if (std::memcmp(bytes.data() + offset, "glTF", 4) != 0)
        throw std::runtime_error("b3dm does not contain a GLB payload");
    const auto length = word(size_t(offset) + 8);
    if (length < 12 || offset + length > bytes.size())
        throw std::runtime_error("Truncated embedded GLB");
    result.glb.assign(bytes.begin() + size_t(offset), bytes.begin() + size_t(offset) + length);
    return result;
}
} // namespace modelassets
