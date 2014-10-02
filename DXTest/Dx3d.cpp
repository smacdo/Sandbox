#include "Dx3d.h"
#include "DXSandbox.h"

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>
#include <vector>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")

Dx3d::Dx3d()
: mInitialized(false),
  mVysncEnabled(false),
  mVideoRamInfo(),
  mVideoCardDescription(),
  mSwapChain(),
  mDevice(),
  mDeviceContext(),
  mRenderTargetView(),
  mDepthStencilTexture(),
  mDepthStencilState(),
  mDepthStencilView(),
  mRasterState(),
  mDepthDisabledStencilState(),
  mAlphaEnabledBlendingState(),
  mAlphaDisabledBlendingState()
{
}

Dx3d::~Dx3d()
{
	Shutdown();
}

void Dx3d::Initialize(int screenWidth,
	                  int screenHeight,
					  bool vsync,
					  HWND hwnd,
					  bool fullscreen,
					  float screenDepth,
					  float screenNear)
{
    screen_size_t screenSize = { screenWidth, screenHeight };
	if (mInitialized)
	{
		return;
	}

	// Store settings.
	mVysncEnabled = vsync;
    mIsFullScreenMode = fullscreen;
    mHwnd = hwnd;

	// Create a DirectX graphics interface factory.
    Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), &factory);

	// Get the primary video card's output adapter.
	Microsoft::WRL::ComPtr<IDXGIOutput> adapterOutput;
    result = GetPrimaryVideoAdapterOutput(factory.Get(), &adapterOutput);

	VerifyDXResult(result);

    // Get monitor refresh information.
    refresh_rate_t refreshRate = { 0 };
    result = GetRefreshRateInfo(adapterOutput.Get(), screenWidth, screenHeight, &refreshRate);

    VerifyDXResult(result);

    // Find out how much video memory is available from our primary video card.
    result = GetPrimaryVideoVRAM(factory.Get(), &mVideoRamInfo);
    VerifyDXResult(result);

	// Create the swap chain, direct3d device and direct3d device context all at once.
    result = CreateDeviceAndSwapChain(
        refreshRate,
        screenWidth,
        screenHeight,
        &mDevice,
        &mDeviceContext,
        &mSwapChain);

	VerifyDXResult(result);

    // Create the render target view.
    result = CreateRenderTargetView(mSwapChain.Get(), &mRenderTargetView);
    VerifyDXResult(result);

    // Set up the back buffer.
    result = InitializeDepthBuffer(mDevice.Get(), screenSize);
    VerifyDXResult(result);

    // Rasterizer state.
    result = CreateRasterizerState(mDevice.Get(), mDeviceContext.Get(), &mRasterState);

	// Set the viewport description which will allow Direct3D to map clip space coordinates to the render target space.
    SetViewport(screenSize);

	// Create the world matrix. This matrix is used to convert model space vertices into the 3d world space. After
	// creating it make sure we initialize to the default identity matrix.
	mWorldMatrix = DirectX::XMMatrixIdentity();

	// Create an orthographic projection matrix for 2d rendering. This matrix will be used to render 2d elements like the
	// user inteface.
	mOrthoMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    // Create a depth stencil state that has the depth stencil disabled.
    result = CreateDepthStencilState(false, mDevice.Get(), &mDepthDisabledStencilState);
    VerifyDXResult(result);

    // Create alpha enabled and disabled blend states.
    result = CreateAlphaBlendState(true, mDevice.Get(), &mAlphaEnabledBlendingState);
    VerifyDXResult(result);

    result = CreateAlphaBlendState(false, mDevice.Get(), &mAlphaDisabledBlendingState);
    VerifyDXResult(result);

	// All done... finally!
	mInitialized = true;

    // TODO: Convert all &mXXX comptr base references to local comptr<t> and only on final success asssign them!
}

HRESULT Dx3d::InitializeDepthBuffer(
    ID3D11Device *pDevice,
    const screen_size_t& screenSize)
{
    AssertNotNull(pDevice);
    Microsoft::WRL::ComPtr<ID3D11Device> device(pDevice);

    // Create depth texture for the depth buffer.
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    MakeDepthTextureDesc(&depthBufferDesc, screenSize);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
    HRESULT hr = device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilTexture);
    
    if (SUCCEEDED(hr))
    {
        // Create depth stencil state object.
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

        hr = CreateDepthStencilStateAndView(
            mDevice.Get(),
            mDeviceContext.Get(),
            mRenderTargetView.Get(),
            depthStencilTexture.Get(),
            &depthStencilState,
            &depthStencilView);

        if (SUCCEEDED(hr))
        {
            // assign out values here. TODO: Maybe move this out to the caller?
            mDepthStencilTexture.Swap(depthStencilTexture);
            mDepthStencilState.Swap(depthStencilState);
            mDepthStencilView.Swap(depthStencilView);
        }
    }

    return hr;
}

// TODO: Invert order, rename pDesc to pDescOut
void Dx3d::MakeDepthTextureDesc(D3D11_TEXTURE2D_DESC *pDesc, const screen_size_t& screenSize) const
{
    AssertNotNull(pDesc);
    ZeroMemory(pDesc, sizeof(D3D11_TEXTURE2D_DESC));

    pDesc->Width = screenSize.width;
    pDesc->Height = screenSize.height;
    pDesc->MipLevels = 1;
    pDesc->ArraySize = 1;
    pDesc->Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pDesc->SampleDesc.Count = 1;
    pDesc->SampleDesc.Quality = 0;
    pDesc->Usage = D3D11_USAGE_DEFAULT;
    pDesc->BindFlags = D3D11_BIND_DEPTH_STENCIL;
    pDesc->CPUAccessFlags = 0;
    pDesc->MiscFlags = 0;
}

HRESULT Dx3d::CreateDepthStencilState(
    bool depthEnabled,
    ID3D11Device *pDevice,
    ID3D11DepthStencilState **ppDepthStencilState) const
{
    AssertNotNull(pDevice);
    AssertNotNull(ppDepthStencilState);

    *ppDepthStencilState = nullptr;

    // Describe the depth stencil state we are trying to create.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = depthEnabled;           // Changed from earlier constructed depth stencil.
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state object using the above description.
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    HRESULT hr = pDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    if (SUCCEEDED(hr))
    {
        *ppDepthStencilState = depthStencilState.Detach();
    }

    return hr;
}

HRESULT Dx3d::CreateDepthStencilStateAndView(
    ID3D11Device * pDevice,
    ID3D11DeviceContext * pDeviceContext,
    ID3D11RenderTargetView * pRenderTargetView,
    ID3D11Texture2D * pDepthBufferTexture,
    ID3D11DepthStencilState **ppDepthStencilState,
    ID3D11DepthStencilView **ppDepthStencilView) const
{
    AssertNotNull(pDevice);
    AssertNotNull(pDeviceContext);
    AssertNotNull(pRenderTargetView);
    AssertNotNull(ppDepthStencilState);
    AssertNotNull(ppDepthStencilView);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

    *ppDepthStencilState = nullptr;
    *ppDepthStencilView = nullptr;

    HRESULT hr = CreateDepthStencilState(true, pDevice, &depthStencilState);
    
    if (SUCCEEDED(hr))
    {
        pDeviceContext->OMSetDepthStencilState(*ppDepthStencilState, 1);

        // Tell Direct3D how to use this buffer buffer as a depth stencil texture.
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // Create the depth stencil view object.
        hr = pDevice->CreateDepthStencilView(pDepthBufferTexture, &depthStencilViewDesc, &depthStencilView);
        
        if (SUCCEEDED(hr))
        {
            std::vector<ID3D11RenderTargetView*> renderTargetViews = { pRenderTargetView };
            pDeviceContext->OMSetRenderTargets(1, &renderTargetViews[0], depthStencilView.Get());

            *ppDepthStencilState = depthStencilState.Detach();
            *ppDepthStencilView = depthStencilView.Detach();
        }
    }

    return hr;
}

HRESULT Dx3d::CreateRasterizerState(
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pDeviceContext,
    ID3D11RasterizerState **ppRasterizerStateOut) const
{
    AssertNotNull(pDevice);
    AssertNotNull(pDeviceContext);
    AssertNotNull(ppRasterizerStateOut);

    *ppRasterizerStateOut = nullptr;

    // Set the raster description object. This allows us to configure how and what polygons will be drawn.
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));

    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    //   rasterDesc.DepthClipEnable = false;

    // Create the raster state.
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    HRESULT hr = pDevice->CreateRasterizerState(&rasterDesc, &rasterizerState);

    if (SUCCEEDED(hr))
    {
        pDeviceContext->RSSetState(rasterizerState.Get());
        *ppRasterizerStateOut = rasterizerState.Detach();
    }

    return hr;
}

/**
 * Get the primary adapter for the primary video card.
 */
HRESULT Dx3d::GetPrimaryVideoAdapterOutput(IDXGIFactory *pDxgiFactory, IDXGIOutput **ppOutputAdapter) const
{
    *ppOutputAdapter = nullptr;
    VerifyNotNull(pDxgiFactory);

    // Use the dxgi factory to create an adapter for the primary graphics interface (video card).
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    HRESULT hr = pDxgiFactory->EnumAdapters(0, &adapter);  // TODO: Make sure there's at least one card.

    if (SUCCEEDED(hr))
    {
        // Get the primary adapter output (monitor).
        Microsoft::WRL::ComPtr<IDXGIOutput> adapterOutput;
        hr = adapter->EnumOutputs(0, &adapterOutput);

        if (SUCCEEDED(hr))
        {
            *ppOutputAdapter = adapterOutput.Detach();
        }
    }
    
    return hr;
}

/**
 * Retrieve monitor refresh rate information for the request display resolution.
 */
HRESULT Dx3d::GetRefreshRateInfo(
    IDXGIOutput * pOutputAdapter,
    unsigned int screenWidth,
    unsigned int screenHeight,
    refresh_rate_t *pRefreshRateOut) const
{
    AssertNotNull(pOutputAdapter);
    AssertNotNull(pRefreshRateOut);

    refresh_rate_t refreshRate;

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the monitor.
    unsigned int numModes = 0;

    HRESULT hr = pOutputAdapter->GetDisplayModeList(
        DXGI_FORMAT_R8G8B8A8_UNORM,         // TODO: Put this in a variable to track since it's used elsewhere.
        DXGI_ENUM_MODES_INTERLACED,
        &numModes,
        NULL);

    if (SUCCEEDED(hr))
    {
        // Create a list to hold all the possible display modes for this monitor / video card combination.
//        std::unique_ptr<DXGI_MODE_DESC[]> displayModeList(new DXGI_MODE_DESC[numModes]);
        std::vector<DXGI_MODE_DESC> displayModeList(numModes);

        hr = pOutputAdapter->GetDisplayModeList(
            DXGI_FORMAT_R8G8B8A8_UNORM,         // TODO: Put this in a variable to track since it's used elsewhere.
            DXGI_ENUM_MODES_INTERLACED,
            &numModes,
            &displayModeList[0]);

        if (SUCCEEDED(hr))
        {
            // Go through all the display modes, and find the one that matches the requested screen width and height.
            // Store a list of the matches along with monitor refresh information.
            for (int i = 0; i < static_cast<int>(numModes); ++i)
            {
                if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
                {
                    refreshRate.numerator = displayModeList[i].RefreshRate.Numerator;
                    refreshRate.denominator = displayModeList[i].RefreshRate.Denominator;
                    break;      // TODO: Confirm??
                }
            }
        }
    }

    *pRefreshRateOut = refreshRate;
    return hr;
}

/**
 * Create, initialize and return the d3d device, context and swap chain.
 */
HRESULT Dx3d::CreateDeviceAndSwapChain(
    const refresh_rate_t& refreshRate,
    unsigned int screenWidth,           // TODO: Replace with size
    unsigned int screenHeight,
    ID3D11Device **ppDeviceOut,
    ID3D11DeviceContext **ppDeviceContextOut,
    IDXGISwapChain **ppSwapChainOut) const
{
    *ppDeviceOut = nullptr;
    *ppDeviceContextOut = nullptr;
    *ppSwapChainOut = nullptr;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.BufferCount = 1;	// Single back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       // TODO: Make this a variable that is tracked.
    swapChainDesc.BufferDesc.RefreshRate.Numerator = (mVysncEnabled ? refreshRate.numerator : 0);
    swapChainDesc.BufferDesc.RefreshRate.Denominator = (mVysncEnabled ? refreshRate.denominator : 1);
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// Backbuffer renders to target.
    swapChainDesc.OutputWindow = mHwnd;
    swapChainDesc.SampleDesc.Count = 1;		// Turn multi-sampling off :(
    swapChainDesc.SampleDesc.Quality = 0;	// Turn multi-sampling off :(   XXX: Fix this, make this configurable.
    swapChainDesc.Windowed = !mIsFullScreenMode;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // XXX: ???
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// XXX: ???
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;				// Discard back buffer contents after presenting.
    swapChainDesc.Flags = 0;											// No need for advanced flags.

    // We want DirectX feature level 11.0.
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create the swap chain, direct3d device and direct3d device context all at once.
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        ppSwapChainOut,
        ppDeviceOut,
        nullptr,
        ppDeviceContextOut);

    return hr;
}

/**
 * Create render target view.
 */
HRESULT Dx3d::CreateRenderTargetView(IDXGISwapChain *pSwapChain, ID3D11RenderTargetView **ppRenderTargetViewOut) const
{
    VerifyNotNull(pSwapChain);
    VerifyNotNull(ppRenderTargetViewOut);

    *ppRenderTargetViewOut = nullptr;

    // Get pointer to the backbuffer.
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);

    if (SUCCEEDED(hr))
    {
        // Create the render target view using our newly aquired back buffer.
        hr = mDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, ppRenderTargetViewOut);
    }
    
    return hr;
}

/**
 * Get video ram information for the primary video card.
 */
HRESULT Dx3d::GetPrimaryVideoVRAM(IDXGIFactory *pDxgiFactory, vram_info_t *pVramInfoOut) const
{
    VerifyNotNull(pDxgiFactory);
    VerifyNotNull(pVramInfoOut);

    *pVramInfoOut = { 0 };
    
    // Use the dxgi factory to create an adapter for the primary graphics interface (video card).
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    HRESULT hr = pDxgiFactory->EnumAdapters(0, &adapter);  // TODO: Make sure there's at least one card.

    if (SUCCEEDED(hr))
    {
        DXGI_ADAPTER_DESC adapterDesc;
        hr = adapter->GetDesc(&adapterDesc);

        if (SUCCEEDED(hr))
        {
            // Report values in MB
            pVramInfoOut->dedicatedVideoMemory = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
            pVramInfoOut->dedicatedSystemMemory = adapterDesc.DedicatedSystemMemory / 1024 / 1024;
            pVramInfoOut->sharedSystemMemory = adapterDesc.SharedSystemMemory / 1024 / 1024;
        }
    }

    return hr;
}

HRESULT Dx3d::CreateAlphaBlendState(
    bool alphaEnabled,
    ID3D11Device *pDevice,
    ID3D11BlendState **ppBlendState) const
{
    AssertNotNull(pDevice);
    AssertNotNull(ppBlendState);

    *ppBlendState = nullptr;

    // Describe the alpha enabled (or disabled) blend state.
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    blendDesc.RenderTarget[0].BlendEnable = alphaEnabled;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    // Create the blend state object.
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
    HRESULT hr = pDevice->CreateBlendState(&blendDesc, &blendState);

    if (SUCCEEDED(hr))
    {
        *ppBlendState = blendState.Detach();
    }

    return hr;
}

void Dx3d::SetViewport(const screen_size_t& screenSize)
{
    D3D11_VIEWPORT viewport;

    viewport.Width = static_cast<float>(screenSize.width);
    viewport.Height = static_cast<float>(screenSize.height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    mDeviceContext->RSSetViewports(1, &viewport);
}

void Dx3d::Shutdown()
{
	if (!mInitialized)
	{
		return;
	}

	// Make sure to switch to windowed mode before releasing the swap chain. Otherwise an exceptiong gets thrown!
	if (mSwapChain.Get() != nullptr)
	{
		mSwapChain->SetFullscreenState(false, NULL);
	}

	mInitialized = false;
}

/// Initialize buffers to be blank and ready.
void Dx3d::BeginScene(float red, float green, float blue, float alpha)
{
	// TODO: Check mInitialized is true.

	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear back buffer and the depth buffer.
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), color);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Dx3d::EndScene()
{
	// Verify mInitialized, and BeginScene called.
	mSwapChain->Present((mVysncEnabled ? 1 : 0), 0);
}

// TODO: Get rid of all these accessor... bad.
ID3D11Device * Dx3d::GetDevice()
{
	return mDevice.Get();
}

ID3D11DeviceContext * Dx3d::GetDeviceContext()
{
	return mDeviceContext.Get();
}

DirectX::SimpleMath::Matrix Dx3d::GetWorldMatrix() const
{
	return mWorldMatrix;
}

DirectX::SimpleMath::Matrix Dx3d::GetOrthoMatrix() const
{
	return mOrthoMatrix;
}

std::string Dx3d::GetVideoCardInfo() const
{
	return mVideoCardDescription;
}

void Dx3d::EnableZBuffer(bool zEnabled)
{
    VerifyNotNull(mDeviceContext.Get());

    if (zEnabled)
    {
        VerifyNotNull(mDepthStencilState.Get());
        mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
    }
    else
    {
        VerifyNotNull(mDepthDisabledStencilState.Get());
        mDeviceContext->OMSetDepthStencilState(mDepthDisabledStencilState.Get(), 1);
    }
}

void Dx3d::EnableAlphaBlending(bool alphaBlendEnabled)
{
    VerifyNotNull(mDeviceContext.Get());
    float blendFactor[4];

    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    if (alphaBlendEnabled)
    {
        mDeviceContext->OMSetBlendState(mAlphaEnabledBlendingState.Get(), blendFactor, 0xFFFFFFFF);
    }
    else
    {
        mDeviceContext->OMSetBlendState(mAlphaDisabledBlendingState.Get(), blendFactor, 0xFFFFFFFF);
    }
}
