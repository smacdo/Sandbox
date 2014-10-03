#pragma once

// heavyweight headers... pimpl?
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <string>
#include <functional>
#include "IInitializable.h"

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

const int VIDEO_CARD_DESC_LENGTH = 128;
class Size;
class Dx3d;
class BinaryBlob;

struct refresh_rate_t
{
    int numerator;
    int denominator;
};

struct vram_info_t      // values in MB
{
    size_t dedicatedVideoMemory;
    size_t dedicatedSystemMemory;
    size_t sharedSystemMemory;
};

enum ShaderType
{
    None,
    Vertex,
    Pixel
};

// TODO: Convert screenWidth / screenHeight to screen_size_t.

// TODO: Give class better name (right now trying to conform closer to tutorial)
//  - maybe DirectXDriver? D3dDriver? hmmm
// TODO: Stop exposing so many DirectX primitives. Wrap them up in proxy objects.
class Dx3d : public IInitializable
{
public:
	Dx3d();
    Dx3d(const Dx3d& rhs) = delete;
	virtual ~Dx3d();

    Dx3d& operator = (const Dx3d& rhs) = delete;

    void Initialize(
        const Size& screenSize,
        HWND hwnd,
        bool isVsyncEnabled,
        bool isFullScreenMode,
        float screenDepth,
        float screenNear);

	void BeginScene();
	void EndScene();

	ID3D11Device * GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
    vram_info_t GetVRamInfo() const;

    void SetBackgroundColor(const DirectX::SimpleMath::Color& backgroundColor);
    void SetZBufferEnabled(bool zEnabled);
    void SetAlphaBlendingEnabled(bool alphaBlendEnabled);

    HRESULT CreateVertexShader(
        const BinaryBlob& shaderBlob,
        ID3D11VertexShader **ppVertexShaderOut) const;

    HRESULT CreatePixelShader(
        const BinaryBlob& shaderBlob,
        ID3D11PixelShader **ppPixelShaderOut) const;

    HRESULT CreateTextureSamplerState(ID3D11SamplerState **ppSamplerStateOut) const;

    // Create a constant buffer.
    HRESULT CreateConstantBuffer(
        size_t elementSize,
        ID3D11Buffer **ppConstantBufferOut) const;

    template<typename T>
    HRESULT UpdateConstantBuffer(
        ID3D11Buffer * pBufferToUpdate,
        ShaderType shaderType,
        int slotIndex,
        std::function<void(T& buffer)> updateFunction) const
    {
        D3D11_MAPPED_SUBRESOURCE mappedChunk;   // TODO: Convert the lock/unlock into a RAII object.
        HRESULT hr = StartUpdatedBuffer(pBufferToUpdate, &mappedChunk);

        if (SUCCEEDED(hr))
        {
            // Let user callback update the buffer.
            T * pTypedPointer = reinterpret_cast<T*>(mappedChunk.pData);
            updateFunction(*pTypedPointer);

            // Unlock the constant buffer.
            FinishUpdateBuffer(shaderType, slotIndex, pBufferToUpdate);
        }

        return hr;
    }

protected:
    virtual void OnShutdown() override;

private:
    HRESULT InitializeDepthBuffer(
        ID3D11Device *pDevice,
        const Size& screenSize);

    void MakeDepthTextureDesc(D3D11_TEXTURE2D_DESC *pDesc, const Size& screenSize) const;

    HRESULT CreateDepthStencilState(
        bool depthEnabled,
        ID3D11Device *pDevice,
        ID3D11DepthStencilState **ppDepthStencilState) const;

    HRESULT CreateDepthStencilStateAndView(
        ID3D11Device * pDevice,
        ID3D11DeviceContext * pDeviceContext,
        ID3D11RenderTargetView * pRenderTargetView,
        ID3D11Texture2D * pDepthBufferTexture,
        ID3D11DepthStencilState **ppDepthStencilState,
        ID3D11DepthStencilView **ppDepthStencilView) const;

    HRESULT CreateRasterizerState(
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pDeviceContext,
        ID3D11RasterizerState **ppRasterizerStateOut) const;

    HRESULT GetPrimaryVideoAdapterOutput(IDXGIFactory *pDxgiFactory, IDXGIOutput **ppOutputAdapter) const;

    // Get monitor refresh info for selected screen size.
    HRESULT GetRefreshRateInfo(
        DXGI_FORMAT displaySurfaceFormat,
        IDXGIOutput * pOutputAdapter,
        const Size& screenSize,
        refresh_rate_t *pRefreshRateOut) const;

    // Create and return the device swap chain.
    HRESULT CreateDeviceAndSwapChain(
        DXGI_FORMAT displaySurfaceFormat,
        const refresh_rate_t& refreshRate,
        const Size& screenSize,
        bool isFullScreenMode,
        ID3D11Device **ppDeviceOut,
        ID3D11DeviceContext **ppDeviceContextOut,
        IDXGISwapChain **ppSwapChainOut) const;

    // Create the render target view.
    HRESULT CreateRenderTargetView(
        IDXGISwapChain *pSwapChain,
        ID3D11RenderTargetView **ppRenderTargetViewOut) const;

    // Get hardware VRAM available.
    HRESULT GetPrimaryVideoVRAM(IDXGIFactory *pDxgiFactory, vram_info_t *pVramInfoOut) const;

    // Create an alpha enabled or disabled blend state.
    HRESULT CreateAlphaBlendState(
        bool alphaEnabled,
        ID3D11Device *pDevice,
        ID3D11BlendState **ppBlendState) const;

    void SetViewport(const Size& screenSize);

    HRESULT StartUpdatedBuffer(
        ID3D11Buffer *pBufferToUpdate,
        D3D11_MAPPED_SUBRESOURCE *pMappedChunkOut) const;

    void FinishUpdateBuffer(
        ShaderType shaderType,
        int slotIndex,
        ID3D11Buffer *pBufferToUpdate) const;

private:
	bool mVysncEnabled;
    HWND mHwnd;                     // TODO: Stop storing this?
    DirectX::SimpleMath::Color mBackgroundColor;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthDisabledStencilState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> mAlphaEnabledBlendingState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> mAlphaDisabledBlendingState;
};

