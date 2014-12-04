#pragma once

#include <string>
#include <memory>
#include <tuple>
#include <ppltasks.h>

struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

namespace DXSandboxApp
{
    class RenderableImageSprite;

    class ResourceLoader
    {
    public:
        ResourceLoader(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ResourceLoader(const ResourceLoader&) = delete;
        ~ResourceLoader();

        ResourceLoader& operator = (const ResourceLoader&) = delete;

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

        // Load and return binary data from a file.
        concurrency::task<std::vector<byte>> LoadDataAsync(const std::wstring& fileName);

        // Load pixel shader from disk.
        concurrency::task<ID3D11PixelShader*> LoadPixelShaderAsync(const std::wstring& fileName);

        // Load vertex shader from disk.
        concurrency::task<ID3D11VertexShader*> LoadVertexShaderAsync(const std::wstring& fileName);

        // Load vertex shader from disk, and generate directx input layout object.
        concurrency::task<std::tuple<ID3D11VertexShader*, ID3D11InputLayout*>> LoadVertexShaderAndCreateInputLayoutAsync(
            const std::wstring& fileName,
            const D3D11_INPUT_ELEMENT_DESC * inputElementDesc,
            unsigned int inputElementCount);

        // Get an image sprite loaded from a file.
        RenderableImageSprite* LoadImageSprite(const std::wstring& imageFileName);

        
        

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> mDeviceResources;
    };
}