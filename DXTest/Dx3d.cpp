#include "Dx3d.h"
#include "DXTest.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")

Dx3d::Dx3d()
: mInitialized(false),
  mVysncEnabled(false),
  mVideoCardMemory(0),
  mVideoCardDescription(),
  mpSwapChain(nullptr),
  mpDevice(nullptr),
  mpDeviceContext(nullptr),
  mpRenderTargetView(nullptr),
  mpDepthStencilBuffer(nullptr),
  mpDepthStencilState(nullptr),
  mpDepthStencilView(nullptr),
  mpRasterState(nullptr),
  mpDepthDisabledStencilState(nullptr),
  mpAlphaEnabledBlendingState(nullptr),
  mpAlphaDisabledBlendingState(nullptr)
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
	HRESULT result;

	if (mInitialized)
	{
		return;
	}

	// Store settings.
	mVysncEnabled = vsync;

	// Create a DirectX graphics interface factory.
	IDXGIFactory * pFactory = nullptr;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	VerifyDXResult(result);
	VerifyNotNull(pFactory);

	// Use the factory to create an adapter for the primary graphics interface (video card).
	IDXGIAdapter * pAdapter = nullptr;

	result = pFactory->EnumAdapters(0, &pAdapter);
	VerifyDXResult(result);
	VerifyNotNull(pAdapter);

	// Get the primary adapter output (monitor).
	IDXGIOutput * pAdapterOutput = nullptr;

	result = pAdapter->EnumOutputs(0, &pAdapterOutput);
	VerifyDXResult(result);
	VerifyNotNull(pAdapterOutput);

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the monitor.
	unsigned int numModes = 0;

	result = pAdapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&numModes,
		NULL);

	VerifyDXResult(result);

	// Create a list to hold all the possible display modes for this monitor / video card combination.
	DXGI_MODE_DESC * pDisplayModeList = new DXGI_MODE_DESC[numModes];
	
	result = pAdapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&numModes,
		pDisplayModeList);

	VerifyDXResult(result);
	VerifyNotNull(pDisplayModeList);

	// Go through all the display modes, and find the one that matches the requested screen width and height. Store a list
	// of the matches along with monitor refresh information.
	int numerator = 0, denominator = 0;

	for (int i = 0; i < static_cast<int>(numModes); ++i)
	{
		if (pDisplayModeList[i].Width == static_cast<unsigned int>(screenWidth) &&
			pDisplayModeList[i].Height == static_cast<unsigned int>(screenHeight))
		{
			numerator = pDisplayModeList[i].RefreshRate.Numerator;
			denominator = pDisplayModeList[i].RefreshRate.Denominator;
	//		break;
		}
	}

	// Retrieve the video card description and VRAM count.
	DXGI_ADAPTER_DESC adapterDesc;

	result = pAdapter->GetDesc(&adapterDesc);
	VerifyDXResult(result);

	// memory in MB
	mVideoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
//	mVideoCardDescription = adapterDesc.Description; // XXX: FIXME

	// Release temporary objects used during initialization.
	SafeDeleteArray(pDisplayModeList);
	SafeRelease(pAdapterOutput);
	SafeRelease(pAdapter);
	SafeRelease(pFactory);

	// Set the desired configuration for our swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;	// Single back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = (mVysncEnabled ? numerator : 0);
	swapChainDesc.BufferDesc.RefreshRate.Denominator = (mVysncEnabled ? denominator : 1);
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// Backbuffer renders to target.
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;		// Turn multi-sampling off :(
	swapChainDesc.SampleDesc.Quality = 0;	// Turn multi-sampling off :(   XXX: Fix this, make this configurable.
	swapChainDesc.Windowed = !fullscreen;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // XXX: ???
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// XXX: ???
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;				// Discard back buffer contents after presenting.
	swapChainDesc.Flags = 0;											// No need for advanced flags.

	// We want DirectX feature level 11.0.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, direct3d device and direct3d device context all at once.
	result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mpSwapChain,
		&mpDevice,
		NULL,
		&mpDeviceContext);

	VerifyDXResult(result);
	VerifyNotNull(mpSwapChain);
	VerifyNotNull(mpDevice);
	VerifyNotNull(mpDeviceContext);

	// Get pointer to the backbuffer.
	ID3D11Texture2D * pBackBuffer = nullptr;
	result = mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	VerifyDXResult(result);
	VerifyNotNull(pBackBuffer);

	// Create the render target view using our newly aquired back buffer.
	result = mpDevice->CreateRenderTargetView(pBackBuffer, NULL, &mpRenderTargetView);

	VerifyDXResult(result);
	VerifyNotNull(mpRenderTargetView);

	// We can release the back buffer pointer since it is no longer needed.
	SafeRelease(pBackBuffer);

	// Set depth buffer description.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	
	// Create the texture for the depth buffer.
	result = mpDevice->CreateTexture2D(&depthBufferDesc, NULL, &mpDepthStencilBuffer);

	VerifyDXResult(result);
	VerifyNotNull(mpDepthStencilBuffer);

	// Set up the depth stencil description.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	// Create a depth stencil state object.
	result = mpDevice->CreateDepthStencilState(&depthStencilDesc, &mpDepthStencilState);

	VerifyDXResult(result);
	VerifyNotNull(mpDepthStencilState);

	mpDeviceContext->OMSetDepthStencilState(mpDepthStencilState, 1);

	// Set the depth stencil view description. This way Direct3D knows to use the depth buffer as a depth stencil texture.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view object.
	result = mpDevice->CreateDepthStencilView(mpDepthStencilBuffer, &depthStencilViewDesc, &mpDepthStencilView);

	VerifyDXResult(result);
	VerifyNotNull(mpDepthStencilView);

	mpDeviceContext->OMSetRenderTargets(1, &mpRenderTargetView, mpDepthStencilView);

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
	result = mpDevice->CreateRasterizerState(&rasterDesc, &mpRasterState);

	VerifyDXResult(result);
	VerifyNotNull(mpRasterState);

	mpDeviceContext->RSSetState(mpRasterState);

	// Set the viewport description which will allow Direct3D to map clip space coordinates to the render target space.
	D3D11_VIEWPORT viewport;

	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	mpDeviceContext->RSSetViewports(1, &viewport);

	// Create the projection matrix. The projection matrix will be used to translate the 3d scene into a 2d viewport space
	// that was created above. We need to keep a copy of this matrix so we can pass it to our shaders.
	float fieldOfView = 0.0f, screenAspect = 0.0f;

	fieldOfView = 3.141592653589793f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// Create the world matrix. This matrix is used to convert model space vertices into the 3d world space. After
	// creating it make sure we initialize to the default identity matrix.
	mWorldMatrix = DirectX::XMMatrixIdentity();

	// Create an orthographic projection matrix for 2d rendering. This matrix will be used to render 2d elements like the
	// user inteface.
	mOrthoMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    // Create a depth stencil state that has the depth stencil disabled.
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

    depthDisabledStencilDesc.DepthEnable = false;           // Changed from earlier constructed depth stencil.
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = mpDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &mpDepthDisabledStencilState);

    VerifyDXResult(result);
    VerifyNotNull(mpDepthDisabledStencilState);

    // Create an alpha enabled blend state.
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    result = mpDevice->CreateBlendState(&blendDesc, &mpAlphaEnabledBlendingState);

    VerifyDXResult(result);
    VerifyNotNull(mpAlphaEnabledBlendingState);

    // Create an alpha disabled blend state.
    blendDesc.RenderTarget[0].BlendEnable = false;

    result = mpDevice->CreateBlendState(&blendDesc, &mpAlphaDisabledBlendingState);

    VerifyDXResult(result);
    VerifyNotNull(mpAlphaDisabledBlendingState);

	// All done... finally!
	mInitialized = true;
}

void Dx3d::Shutdown()
{
	if (!mInitialized)
	{
		return;
	}

	// Make sure to switch to windowed mode before releasing the swap chain. Otherwise an exceptiong gets thrown!
	if (mpSwapChain != nullptr)
	{
		mpSwapChain->SetFullscreenState(false, NULL);
	}

    SafeRelease(mpAlphaEnabledBlendingState);
    SafeRelease(mpAlphaDisabledBlendingState);
    SafeRelease(mpDepthDisabledStencilState);
	SafeRelease(mpRasterState);
	SafeRelease(mpDepthStencilView);
	SafeRelease(mpDepthStencilState);
	SafeRelease(mpDepthStencilBuffer);
	SafeRelease(mpRenderTargetView);
	SafeRelease(mpDeviceContext);
	SafeRelease(mpDevice);
	SafeRelease(mpSwapChain);

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
	mpDeviceContext->ClearRenderTargetView(mpRenderTargetView, color);
	mpDeviceContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Dx3d::EndScene()
{
	// Verify mInitialized, and BeginScene called.
	mpSwapChain->Present((mVysncEnabled ? 1 : 0), 0);
}

// TODO: Get rid of all these accessor... bad.
ID3D11Device * Dx3d::GetDevice()
{
	return mpDevice;
}

ID3D11DeviceContext * Dx3d::GetDeviceContext()
{
	return mpDeviceContext;
}

DirectX::SimpleMath::Matrix Dx3d::GetProjectionMatrix() const
{
	return mProjectionMatrix;
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
    VerifyNotNull(mpDeviceContext);

    if (zEnabled)
    {
        VerifyNotNull(mpDepthStencilState);
        mpDeviceContext->OMSetDepthStencilState(mpDepthStencilState, 1);
    }
    else
    {
        VerifyNotNull(mpDepthDisabledStencilState);
        mpDeviceContext->OMSetDepthStencilState(mpDepthDisabledStencilState, 1);
    }
}

void Dx3d::EnableAlphaBlending(bool alphaBlendEnabled)
{
    VerifyNotNull(mpDeviceContext);
    float blendFactor[4];

    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    if (alphaBlendEnabled)
    {
        mpDeviceContext->OMSetBlendState(mpAlphaEnabledBlendingState, blendFactor, 0xFFFFFFFF);
    }
    else
    {
        mpDeviceContext->OMSetBlendState(mpAlphaDisabledBlendingState, blendFactor, 0xFFFFFFFF);
    }
}
