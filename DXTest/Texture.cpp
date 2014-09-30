#include "Texture.h"
#include "BinaryBlob.h"
#include "DXSandbox.h"
#include <d3d11.h>
#include "DDSTextureLoader.h"

using namespace DirectX;

Texture::Texture()
: mpResource(nullptr),
  mpTexture(nullptr)
{
}

Texture::~Texture()
{
}

void Texture::Initialize(ID3D11Device *pDevice, const std::wstring& filepath)
{
	VerifyNotNull(pDevice);

	if (!IsInitialized())
	{
		HRESULT result = CreateDDSTextureFromFile(pDevice, filepath.c_str(), nullptr, &mpTexture);
		
		VerifyDXResult(result);
		VerifyNotNull(mpTexture);

        // Set an identifier on this texture file for easier debug tracking.
        //  TODO: Broken, probably because filepath.c_str() is not valid later.
        mpTexture->SetPrivateData(WKPDID_D3DDebugObjectName,
                                  sizeof(std::string::value_type) * filepath.size(),
                                  filepath.c_str());

		SetInitialized();
	}
}

void Texture::OnShutdown()
{
	SafeRelease(mpTexture);
	SafeRelease(mpResource);
}

ID3D11ShaderResourceView * Texture::GetTexture()
{
	return mpTexture;
}