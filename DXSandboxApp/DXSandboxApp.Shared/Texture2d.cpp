#include "pch.h"
#include "Common/Debugging.h"
#include "Texture2d.h"

using namespace DXSandboxApp;

Texture2d::Texture2d()
    : mLoaded(false)
{
}

Texture2d::Texture2d(ID3D11Texture2D * pTexture, ID3D11ShaderResourceView * pResourceView)
    : mLoaded(false)
{
    Initialize(pTexture, pResourceView);
}

Texture2d::~Texture2d()
{
    Reset();
}

void Texture2d::Initialize(ID3D11Texture2D * pTexture, ID3D11ShaderResourceView * pResourceView)
{
    CheckNotNull(pTexture);
    CheckNotNull(pResourceView);

    Reset();

    mTexture = pTexture;
    mShaderResourceView = pResourceView;
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
    // TODO: Throw if null.
    return mTexture.Get();
}

const ID3D11Texture2D * Texture2d::GetTexture2D() const
{
    // TODO: Throw if null.
    return mTexture.Get();
}

ID3D11ShaderResourceView * Texture2d::GetShaderResourceView()
{
    // TODO: Throw if null.
    return mShaderResourceView.Get();
}

const ID3D11ShaderResourceView * Texture2d::GetShaderResourceView() const
{
    // TODO: Throw if null.
    return mShaderResourceView.Get();
}