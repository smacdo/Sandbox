#pragma once
#include "IInitializable.h"

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

#include <string>

struct ID3D11Device;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;
class BinaryBlob;

class Texture : public IInitializable
{
public:
	Texture();
	Texture(const Texture& texture) = delete;
	virtual ~Texture() override;

    Texture& operator =(const Texture& rhs) = delete;

	// texture must be dds
	// TODO: convert to use BinaryBlob* rather than manually loading
    void InitializeFromFile(ID3D11Device *pDevice, const std::wstring& filepath);

	ID3D11ShaderResourceView * GetShaderResourceView();
    const ID3D11ShaderResourceView * GetShaderResourceView() const;

private:
	virtual void OnShutdown() override;

private:
    std::wstring mName;
	Microsoft::WRL::ComPtr<ID3D11Resource> mResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
};

