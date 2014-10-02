#pragma once

// heavyweight headers... pimpl?
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <string>

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

const int VIDEO_CARD_DESC_LENGTH = 128;

struct refresh_rate_t
{
    int numerator;
    int denominator;
};

struct screen_size_t
{
    unsigned int width;
    unsigned int height;
};

struct vram_info_t      // values in MB
{
    size_t dedicatedVideoMemory;
    size_t dedicatedSystemMemory;
    size_t sharedSystemMemory;
};

// TODO: Convert screenWidth / screenHeight to screen_size_t.

// TODO: Give class better name (right now trying to conform closer to tutorial)
//  - maybe DirectXDriver? D3dDriver? hmmm
// TODO: Stop exposing so many DirectX primitives. Wrap them up in proxy objects.
class Dx3d
{
public:
	Dx3d();
	~Dx3d();

	void Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device * GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

    // TODO: Merge these functions with a bool parameter.
    void EnableZBuffer(bool zEnabled);
    void EnableAlphaBlending(bool alphaBlendEnabled);

private:
	Dx3d(const Dx3d&);
	Dx3d& operator= (const Dx3d&);

    HRESULT InitializeDepthBuffer(
        ID3D11Device *pDevice,
        const screen_size_t& screenSize);

    void MakeDepthTextureDesc(D3D11_TEXTURE2D_DESC *pDesc, const screen_size_t& screenSize) const;

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
        IDXGIOutput * pOutputAdapter,
        unsigned int screenWidth,           // TODO: replace with size
        unsigned int screenHeight,
        refresh_rate_t *pRefreshRateOut) const;

    // Create and return the device swap chain.
    HRESULT CreateDeviceAndSwapChain(
        const refresh_rate_t& refreshRate,
        unsigned int screenWidth,           // TODO: Replace with size
        unsigned int screenHeight,
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

    void SetViewport(const screen_size_t& screenSize);

private:
	bool mInitialized;
	bool mVysncEnabled;
    bool mIsFullScreenMode;
    HWND mHwnd;
    vram_info_t mVideoRamInfo;
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

