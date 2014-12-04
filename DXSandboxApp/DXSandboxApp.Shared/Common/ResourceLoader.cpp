#include "pch.h"
#include "ResourceLoader.h"
#include <ppltasks.h>

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

concurrency::task<std::vector<byte>> ResourceLoader::LoadDataAsync(const std::wstring& fileName)
{
    using namespace Windows::Storage;
    using namespace Concurrency;

    auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

    return create_task(folder->GetFileAsync(Platform::StringReference(fileName.c_str()))).then([](StorageFile^ file)
    {
        return FileIO::ReadBufferAsync(file);
    }).then([](Streams::IBuffer^ fileBuffer) -> std::vector<byte>
    {
        std::vector<byte> returnBuffer;
        returnBuffer.resize(fileBuffer->Length);
        Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), fileBuffer->Length));
        return returnBuffer;
    });
}

/**
 * \brief Load pixel shader from file.
 */
concurrency::task<ID3D11PixelShader*> ResourceLoader::LoadPixelShaderAsync(const std::wstring& fileName)
{
    auto loadPSTask = LoadDataAsync(fileName);

    return loadPSTask.then([this](const std::vector<byte>& fileData) {
        ID3D11PixelShader * pixelShader = nullptr;

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreatePixelShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &pixelShader));

        return pixelShader;
    });
}

/**
 * \brief Load vertex shader from disk.
 */
concurrency::task<ID3D11VertexShader*> ResourceLoader::LoadVertexShaderAsync(const std::wstring& fileName)
{
    auto loadVSTask = LoadDataAsync(fileName);

    return loadVSTask.then([this](const std::vector<byte>& fileData) {
        ID3D11VertexShader * vertexShader = nullptr;

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateVertexShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &vertexShader));

        return vertexShader;
    });
}

/**
* \brief Load vertex shader from disk, and create input layout from the shader.
*/
concurrency::task<std::tuple<ID3D11VertexShader*, ID3D11InputLayout*>>  ResourceLoader::LoadVertexShaderAndCreateInputLayoutAsync(
    const std::wstring& fileName,
    const D3D11_INPUT_ELEMENT_DESC * inputElementDesc,
    unsigned int inputElementCount)
{
    auto loadVSTask = LoadDataAsync(fileName);

    return loadVSTask.then([this, inputElementDesc, inputElementCount](const std::vector<byte>& fileData) {
        ID3D11VertexShader * vertexShader = nullptr;
        ID3D11InputLayout * inputLayout = nullptr;

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateVertexShader(
                &fileData[0],
                fileData.size(),
                nullptr,
                &vertexShader));

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateInputLayout(
                inputElementDesc,
                inputElementCount,
                &fileData[0],
                fileData.size(),
                &inputLayout));

        return std::make_tuple(vertexShader, inputLayout);
    });
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