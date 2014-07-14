#pragma once

// heavyweight headers... pimpl?
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <string>

const int VIDEO_CARD_DESC_LENGTH = 128;

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

	DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
	DirectX::SimpleMath::Matrix GetWorldMatrix() const;
	DirectX::SimpleMath::Matrix GetOrthoMatrix() const;
	std::string GetVideoCardInfo() const;

    // TODO: Merge these functions with a bool parameter.
    void EnableZBuffer(bool zEnabled);
    void EnableAlphaBlending(bool alphaBlendEnabled);

private:
	Dx3d(const Dx3d&);
	Dx3d& operator= (const Dx3d&);

private:
	bool mInitialized;
	bool mVysncEnabled;
	int mVideoCardMemory;
	std::string mVideoCardDescription;
	IDXGISwapChain * mpSwapChain;
	ID3D11Device * mpDevice;
	ID3D11DeviceContext * mpDeviceContext;
	ID3D11RenderTargetView * mpRenderTargetView;
	ID3D11Texture2D * mpDepthStencilBuffer;
	ID3D11DepthStencilState * mpDepthStencilState;
	ID3D11DepthStencilView * mpDepthStencilView;
	ID3D11RasterizerState * mpRasterState;
	DirectX::SimpleMath::Matrix mProjectionMatrix;
	DirectX::SimpleMath::Matrix mWorldMatrix;
	DirectX::SimpleMath::Matrix mOrthoMatrix;
    ID3D11DepthStencilState * mpDepthDisabledStencilState;
    ID3D11BlendState * mpAlphaEnabledBlendingState;
    ID3D11BlendState * mpAlphaDisabledBlendingState;
};

