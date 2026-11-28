#include "ModelAssetsInternal.h"
#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <stdexcept>
#include <cstring>
using Microsoft::WRL::ComPtr;
namespace modelassets
{
Texture DecodeImage(const unsigned char *bytes, size_t size)
{
    if (!bytes || !size || size > UINT_MAX)
        throw std::runtime_error("Invalid texture length");
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    struct Scope
    {
        HRESULT h;
        ~Scope()
        {
            if (SUCCEEDED(h))
                CoUninitialize();
        }
    } scope{initialized};
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IWICStream> stream;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> converter;
    auto require = [](HRESULT result) {
        if (FAILED(result))
            throw std::runtime_error("Texture decoding failed (PNG/JPEG/BMP/TIFF supported by WIC)");
    };
    require(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));
    require(factory->CreateStream(&stream));
    require(stream->InitializeFromMemory(const_cast<BYTE *>(bytes), static_cast<DWORD>(size)));
    require(factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder));
    require(decoder->GetFrame(0, &frame));
    Texture result;
    require(frame->GetSize(&result.width, &result.height));
    if (!result.width || !result.height || result.width > 8192 || result.height > 8192)
        throw std::runtime_error("Texture dimensions exceed 8192 pixels");
    require(factory->CreateFormatConverter(&converter));
    require(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr,
                                  0, WICBitmapPaletteTypeCustom));
    result.rgba.resize(size_t(result.width) * result.height * 4);
    require(converter->CopyPixels(nullptr, result.width * 4, static_cast<UINT>(result.rgba.size()),
                                  result.rgba.data()));
    const auto stride = size_t(result.width) * 4;
    for (uint32_t y = 0; y < result.height / 2; ++y)
        for (size_t x = 0; x < stride; ++x)
            std::swap(result.rgba[y * stride + x], result.rgba[(result.height - y - 1) * stride + x]);
    return result;
}
} // namespace modelassets
