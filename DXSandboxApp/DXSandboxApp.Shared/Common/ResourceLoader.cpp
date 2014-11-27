#include "pch.h"
#include "ResourceLoader.h"

#include "Common/DirectXHelper.h"
#include "Ui/RenderableImageSprite.h"

using namespace DXSandboxApp;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

ResourceLoader::ResourceLoader(const std::shared_ptr<DX::DeviceResources>& deviceResources)
    : mDeviceResources(deviceResources)
{
}

ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::CreateDeviceDependentResources()
{
}

RenderableImageSprite* ResourceLoader::LoadImageSprite(const std::wstring& imageFileName)
{
    // Format path to image file name.
    auto location = Package::Current->InstalledLocation;
    
    Platform::String^ path = Platform::String::Concat(location->Path, "\\");
    path = Platform::String::Concat(path, "Assets\\");
    path = Platform::String::Concat(path, Platform::StringReference(imageFileName.c_str()));

    // Get WIC imaging factory, and create a decoder for the requested image.
    auto wicFactory = mDeviceResources->GetWicImagingFactory();

    ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
    DX::ThrowIfFailed(
        wicFactory->CreateDecoderFromFilename(
            path->Data(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &wicBitmapDecoder));

    ComPtr<IWICBitmapFrameDecode> wicBitmapFrame;
    DX::ThrowIfFailed(wicBitmapDecoder->GetFrame(0, &wicBitmapFrame));

    ComPtr<IWICFormatConverter> wicFormatConverter;
    DX::ThrowIfFailed(wicFactory->CreateFormatConverter(&wicFormatConverter));

    // Initialize WIC decoder. Set the output format to be the same as the formated used by the engine when rendering
    // 2d sprites.
    DX::ThrowIfFailed(
        wicFormatConverter->Initialize(
            wicBitmapFrame.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom));  // The BGRA format has no palette so this value is ignored.

    // Get image DPI data. (I'd be surprised if this ever changes...)
    double dpiX = 96.0f;
    double dpiY = 96.0f;

    DX::ThrowIfFailed(wicFormatConverter->GetResolution(&dpiX, &dpiY));

    // Create D2D Resources
    ComPtr<ID2D1Bitmap> imageBitmap;
    D2D1_SIZE_F imageSize;
    auto d2dContext = mDeviceResources->GetD2DDeviceContext();

    DX::ThrowIfFailed(
        d2dContext->CreateBitmapFromWicBitmap(
            wicFormatConverter.Get(),
            BitmapProperties(
                PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                static_cast<float>(dpiX),
                static_cast<float>(dpiY)),
            &imageBitmap));

    imageSize = imageBitmap->GetSize();

    return new RenderableImageSprite(mDeviceResources, imageBitmap.Get(), imageSize.width, imageSize.height);
}

void ResourceLoader::ReleaseDeviceDependentResources()
{
}