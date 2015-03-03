#include "pch.h"
#include "Common\Utility.h"
#include "Material.h"

using namespace DXSandboxApp;

Material::Material()
    : mLoaded(false)
{
}

Material::Material(
    ID3D11Texture2D * pTexture,
    ID3D11ShaderResourceView * pResourceView,
    ID3D11SamplerState * pSamplerState)
    : mLoaded(false)
{
    Initialize(pTexture, pResourceView, pSamplerState);
}

Material::~Material()
{
    Reset();
}

void Material::Initialize(
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

void Material::Reset()
{
    mTexture = nullptr;
    mShaderResourceView = nullptr;
    mLoaded = false;
}

ID3D11Texture2D * Material::GetTexture()
{
    return mTexture.Get();
}

const ID3D11Texture2D * Material::GetTexture() const
{
    return mTexture.Get();
}

ID3D11ShaderResourceView * Material::GetShaderResourceView()
{
    return mShaderResourceView.Get();
}

const ID3D11ShaderResourceView * Material::GetShaderResourceView() const
{
    return mShaderResourceView.Get();
}

ID3D11SamplerState * Material::GetSamplerState()
{
    return mSamplerState.Get();
}

const ID3D11SamplerState * Material::GetSamplerState() const
{
    return mSamplerState.Get();
}

void Material::SetMeshColor(DirectX::XMFLOAT4 color)
{
    mMeshColor = color;
}

void Material::SetDiffuseColor(DirectX::XMFLOAT4 color)
{
    mDiffuseColor = color;
}

void Material::SetSpecularColor(DirectX::XMFLOAT4 color)
{
    mSpecularColor = color;
}

void Material::SetSpecularExponent(float value)
{
    mSpecularExponent = value;
}

DirectX::XMFLOAT4 Material::MeshColor() const
{
    return mMeshColor;
}

DirectX::XMFLOAT4 Material::DiffuseColor() const
{
    return mDiffuseColor;
}

DirectX::XMFLOAT4 Material::SpecularColor() const
{
    return mSpecularColor;
}

float Material::SpecularExponent() const
{
    return mSpecularExponent;
}