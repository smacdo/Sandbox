#include "Texture.h"
#include "BinaryBlob.h"
#include "DXSandbox.h"
#include "DXTestException.h"

#include <d3d11.h>
#include "DDSTextureLoader.h"

using namespace DirectX;

Texture::Texture()
    : mName(),
      mResource(),
      mTexture()
{
}

Texture::~Texture()
{
}

void Texture::InitializeFromFile(ID3D11Device *pDevice, const std::wstring& filepath)
{
    if (IsInitialized()) { return; }
	VerifyNotNull(pDevice);

    Microsoft::WRL::ComPtr<ID3D11Resource> textureResource;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

    HRESULT hr = CreateDDSTextureFromFile(pDevice, filepath.c_str(), &textureResource, &shaderResourceView);

    if (SUCCEEDED(hr))
    {
        mName = filepath;
        mResource.Swap(textureResource);
        mTexture.Swap(shaderResourceView);

        // Set an identifier on this texture file for easier debug tracking.
        //  TODO: Confirm this works.
        mTexture->SetPrivateData(WKPDID_D3DDebugObjectName,
                                 sizeof(std::wstring::value_type) * mName.size(),
                                 mName.c_str());

        SetInitialized();
    }
    else
    {
        throw DirectXException(hr, filepath);
    }
}

void Texture::OnShutdown()
{
}

ID3D11ShaderResourceView * Texture::GetShaderResourceView()
{
    if (!IsInitialized()) { throw NotInitializedException(L"Texture"); }
	return mTexture.Get();
}

const ID3D11ShaderResourceView * Texture::GetShaderResourceView() const
{
    if (!IsInitialized()) { throw NotInitializedException(L"Texture"); }
    return mTexture.Get();
}