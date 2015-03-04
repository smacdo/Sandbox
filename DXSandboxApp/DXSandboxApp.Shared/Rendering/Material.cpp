#include "pch.h"
#include "Common\Utility.h"
#include "Material.h"
#include "Rendering\Texture.h"

#include <memory>

using namespace DXSandboxApp;

Material::Material()
    : mLoaded(false)
{
}

Material::Material(
	std::shared_ptr<Texture2d> texture,
    ID3D11SamplerState * pSamplerState)
    : mLoaded(false)
{
    Initialize(texture, pSamplerState);
}

Material::~Material()
{
    Reset();
}

void Material::Initialize(
	std::shared_ptr<Texture2d> texture,
    ID3D11SamplerState * pSamplerState)
{
    CheckNotNull(pSamplerState);
    Reset();

    mTexture = texture;
    mSamplerState = pSamplerState;
    mLoaded = true;

	SetDebugName(pSamplerState, texture->Name() + L"(Sampler State)");
}

void Material::Reset()
{
    mTexture = nullptr;
    mLoaded = false;
}

ID3D11Texture2D * Material::GetTexture()
{
	return mTexture->GetTexture();
}

const ID3D11Texture2D * Material::GetTexture() const
{
	return mTexture->GetTexture();
}

ID3D11ShaderResourceView * Material::GetShaderResourceView()
{
	return mTexture->GetShaderResourceView();
}

const ID3D11ShaderResourceView * Material::GetShaderResourceView() const
{
	return mTexture->GetShaderResourceView();
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