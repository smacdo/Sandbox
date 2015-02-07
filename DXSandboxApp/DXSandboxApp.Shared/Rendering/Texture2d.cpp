#include "pch.h"
#include "Common/Debugging.h"
#include "Texture2d.h"

using namespace DXSandboxApp;

Texture2d::Texture2d()
    : mLoaded(false)
{
}

Texture2d::Texture2d(
    ID3D11Texture2D * pTexture,
    ID3D11ShaderResourceView * pResourceView,
    ID3D11SamplerState * pSamplerState)
    : mLoaded(false)
{
    Initialize(pTexture, pResourceView, pSamplerState);
}

Texture2d::~Texture2d()
{
    Reset();
}

void Texture2d::Initialize(
    ID3D11Texture2D * pTexture,
    ID3D11ShaderResourceView * pResourceView,
    ID3D11SamplerState * pSamplerState)
{
    CheckNotNull(pTexture);
    CheckNotNull(pResourceView);

    Reset();

    mTexture = pTexture;
    mShaderResourceView = pResourceView;
    mSamplerState = pSamplerState;
    mLoaded = true;
}

void Texture2d::Reset()
{
    mTexture = nullptr;
    mShaderResourceView = nullptr;
    mLoaded = false;
}

ID3D11Texture2D * Texture2d::GetTexture2D()
{
    return mTexture.Get();
}

const ID3D11Texture2D * Texture2d::GetTexture2D() const
{
    return mTexture.Get();
}

ID3D11ShaderResourceView * Texture2d::GetShaderResourceView()
{
    return mShaderResourceView.Get();
}

const ID3D11ShaderResourceView * Texture2d::GetShaderResourceView() const
{
    return mShaderResourceView.Get();
}

ID3D11SamplerState * Texture2d::GetSamplerState()
{
    return mSamplerState.Get();
}

const ID3D11SamplerState * Texture2d::GetSamplerState() const
{
    return mSamplerState.Get();
}