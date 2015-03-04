#include "pch.h"
#include "Texture.h"
#include "Common\DirectXHelper.h"

using namespace DXSandboxApp;

Texture2d::Texture2d()
{
}

Texture2d::Texture2d(
	const std::wstring& name,
	ID3D11Texture2D * pTexture,
	ID3D11ShaderResourceView * pShaderResourceView)
{
	Initialize(name, pTexture, pShaderResourceView);
}

Texture2d::~Texture2d()
{
}

void Texture2d::Initialize(
	const std::wstring& name,
	ID3D11Texture2D * pTexture,
	ID3D11ShaderResourceView * pShaderResourceView)
{
	CheckNotNull(pTexture);
	CheckNotNull(pShaderResourceView);

	mName = name;
	mTexture = pTexture;
	mShaderResourceView = pShaderResourceView;

	// Set debug names.
	SetDebugName(mTexture.Get(), name);
	SetDebugName(mShaderResourceView.Get(), name + L" (SRV)");
}

std::wstring Texture2d::Name() const
{
	return mName;
}

ID3D11Texture2D * Texture2d::GetTexture()
{
	return mTexture.Get();
}

ID3D11Texture2D * Texture2d::GetTexture() const
{
	return mTexture.Get();
}

ID3D11ShaderResourceView * Texture2d::GetShaderResourceView()
{
	return mShaderResourceView.Get();
}

ID3D11ShaderResourceView * Texture2d::GetShaderResourceView() const
{
	return mShaderResourceView.Get();
}
