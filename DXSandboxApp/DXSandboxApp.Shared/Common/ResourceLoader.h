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
    class SimpleMesh;
    class Texture2d;

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

        // Load and return text data from a file.
        concurrency::task<std::wstring> LoadWideStringAsync(const std::wstring& fileName);

        // Load and return text data from a file.
        concurrency::task<std::string> LoadStringAsync(const std::wstring& fileName);

        // Load pixel shader from disk.
        concurrency::task<ID3D11PixelShader*> LoadPixelShaderAsync(const std::wstring& fileName);

        // Load vertex shader from disk.
        concurrency::task<ID3D11VertexShader*> LoadVertexShaderAsync(const std::wstring& fileName);

        // Load vertex shader from disk, and generate directx input layout object.
        concurrency::task<std::tuple<ID3D11VertexShader*, ID3D11InputLayout*>> LoadVertexShaderAndCreateInputLayoutAsync(
            _In_ const std::wstring& fileName,
            _In_reads_bytes_(inputElementCount) const D3D11_INPUT_ELEMENT_DESC * inputElementDesc,
            _In_ uint32 inputElementCount);

        // Load texture from disk.
        concurrency::task<Texture2d*> LoadTexture2dAsync(const std::wstring& fileName);

        // Load texture from disk.
        Texture2d* LoadTexture2d(const std::wstring& fileName);

        // Get an image sprite loaded from a file.
        concurrency::task<RenderableImageSprite*> LoadImageSpriteAsync(const std::wstring& imageFileName);

        // Get an image sprite loaded from a file.
        RenderableImageSprite* LoadImageSprite(const std::wstring& imageFileName);

        static std::wstring ToWideString(const std::string& string);
        static std::string ToUtf8String(const std::wstring& string);

    protected:
        void CreateSimpleModelInputLayout(
            _In_reads_bytes_(bytecodeSize) byte *bytecode,
            _In_ uint32 bytecodeSize,
            _Out_ ID3D11InputLayout ** layoutOut);

        void CreateInputLayout(
            _In_reads_bytes_(bytecodeSize) byte *bytecode,
            _In_ uint32 bytecodeSize,
            _In_reads_bytes_(layoutDescNumElements) const D3D11_INPUT_ELEMENT_DESC *layoutDesc,
            _In_ uint32 layoutDescNumElements,
            _Out_ ID3D11InputLayout ** layoutOut);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> mDeviceResources;
    };
}