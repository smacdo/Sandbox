#include "pch.h"
#include "ResourceLoader.h"
#include <ppltasks.h>
#include <string>
#include <locale>
#include <memory>
#include <codecvt>

#include "Common/DirectXHelper.h"
#include "Ui/RenderableImageSprite.h"
#include "Rendering/Material.h"
#include "Rendering/ConfigurableDesc.h"

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

/**
 * \brief Load a byte array from a file.
 */
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
* \brief Load a wide text string from a file.
*/
concurrency::task<std::wstring> ResourceLoader::LoadWideStringAsync(const std::wstring& fileName)
{
    using namespace Windows::Storage;
    using namespace Concurrency;

    auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

    return create_task(folder->GetFileAsync(Platform::StringReference(fileName.c_str()))).then([](StorageFile^ file)
    {
        return FileIO::ReadBufferAsync(file);
    }).then([](Streams::IBuffer^ fileBuffer) -> std::wstring
    {
        // Load HString string from buffer.
        Platform::String^ nativeText = Streams::DataReader::FromBuffer(fileBuffer)->ReadString(fileBuffer->Length);

        return std::wstring(nativeText->Data());
    });
}

/**
 * \brief Load a text string from a file.
 */
concurrency::task<std::string> ResourceLoader::LoadStringAsync(const std::wstring& fileName)
{
    return LoadWideStringAsync(fileName).then([](std::wstring wideString){
        return ToUtf8String(wideString);
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
    _In_ const std::wstring& fileName,
    _In_reads_bytes_(inputElementCount) const D3D11_INPUT_ELEMENT_DESC * inputElementDesc,
    _In_ uint32 inputElementCount)
{
    auto loadVSTask = LoadDataAsync(fileName);

    return loadVSTask.then([this, inputElementDesc, inputElementCount](const std::vector<byte>& fileData) {
        ID3D11VertexShader * vertexShader = nullptr;
        ID3D11InputLayout * inputLayout = nullptr;

        // TODO: Call CreateInputLayout instead of doing it here.

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

concurrency::task<Material *> ResourceLoader::LoadTexture2dAsync(
    const std::wstring& fileName,
    const SamplerSettings& settings)
{
    using namespace Concurrency;
    return create_task([=] { return LoadTexture2d(fileName, settings); });
}

Material * ResourceLoader::LoadTexture2d(
    const std::wstring& fileName,
    const SamplerSettings& settings)
{
    // Format path to image file name.
    auto location = Package::Current->InstalledLocation;

    Platform::String^ path = Platform::String::Concat(location->Path, "\\");
    path = Platform::String::Concat(path, "Assets\\");
    path = Platform::String::Concat(path, Platform::StringReference(fileName.c_str()));

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

    // Read image into temporary pixel buffer.
    uint32_t width = 0, height = 0;
    DX::ThrowIfFailed(wicBitmapFrame->GetSize(&width, &height));

    std::unique_ptr<uint8_t[]> pixels(new uint8_t[width * height * 4]);
    DX::ThrowIfFailed(
        wicFormatConverter->CopyPixels(
            nullptr,                // Read from the entire bitmap, not a subregion.
            width * 4,              // Image stride.
            width * height * 4,     // Size of the image buffer.
            pixels.get()));         // Destination image buffer pointer.

    // Configure and initialize Direct2d texture object from pixel data.
    D3D11_SUBRESOURCE_DATA initialData = {0};
    initialData.pSysMem = pixels.get();
    initialData.SysMemPitch = width * 4;
    initialData.SysMemSlicePitch = 0;

    CD3D11_TEXTURE2D_DESC textureDesc(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        width,
        height,
        1,
        1);

    ComPtr<ID3D11Texture2D> texture2d;
    DX::ThrowIfFailed(
        mDeviceResources->GetD3DDevice()->CreateTexture2D(
            &textureDesc,
            &initialData,
            &texture2d));

    // Create a shader resource view object as well.
    ComPtr<ID3D11ShaderResourceView> srv;
    CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(
        texture2d.Get(),
        D3D11_SRV_DIMENSION_TEXTURE2D);

    DX::ThrowIfFailed(
        mDeviceResources->GetD3DDevice()->CreateShaderResourceView(
            texture2d.Get(),
            &srvDesc,
            &srv));

    // Create a default sampler state.
    ComPtr<ID3D11SamplerState> sampler;
    sampler = settings.CreateSamplerState(mDeviceResources->GetD3DDevice());

    // TODO: Set debug name on texture.

    // Create and return a new shader object.
    return new Material(texture2d.Get(), srv.Get(), sampler.Get());
}

concurrency::task<RenderableImageSprite*> ResourceLoader::LoadImageSpriteAsync(const std::wstring& fileName)
{
    using namespace Concurrency;
    return create_task([=] { return LoadImageSprite(fileName); });
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

std::wstring ResourceLoader::ToWideString(const std::string& string)
{
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.from_bytes(string);
}

std::string ResourceLoader::ToUtf8String(const std::wstring& string)
{
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.to_bytes(string);
}


/**
* Generate input layout for simple model format.
*/
void ResourceLoader::CreateSimpleModelInputLayout(
    _In_reads_bytes_(bytecodeSize) byte *bytecode,
    _In_ uint32 bytecodeSize,
    _Out_ ID3D11InputLayout ** layoutOut)
{
    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =        // TODO: Make separate function.
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    CreateInputLayout(bytecode, bytecodeSize, &vertexDesc[0], ARRAYSIZE(vertexDesc), layoutOut);
}

/**
* Generate input layout for simple model format.
*/
void ResourceLoader::CreateInputLayout(
    _In_reads_bytes_(bytecodeSize) byte *bytecode,
    _In_ uint32 bytecodeSize,
    _In_reads_bytes_(layoutDescNumElements) const D3D11_INPUT_ELEMENT_DESC *layoutDesc,
    _In_ uint32 layoutDescNumElements,
    _Out_ ID3D11InputLayout ** layoutOut)
{
    DX::ThrowIfFailed(
        mDeviceResources->GetD3DDevice()->CreateInputLayout(
        layoutDesc,
        layoutDescNumElements,
        bytecode,
        bytecodeSize,
        layoutOut));
}