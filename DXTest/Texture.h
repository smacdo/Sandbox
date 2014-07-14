#pragma once
#include "IInitializable.h"

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

	// texture must be dds
	// TODO: convert to use BinaryBlob* rather than manually loading
	void Initialize(ID3D11Device *pDevice, const std::wstring& filepath);

	// TODO: bad name
	ID3D11ShaderResourceView * GetTexture();

	Texture& operator =(const Texture& rhs) = delete;

private:
	virtual void OnShutdown() override;

private:
	ID3D11Resource * mpResource;
	ID3D11ShaderResourceView * mpTexture;
};

