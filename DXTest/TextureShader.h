#pragma once
#include "SimpleMath.h"
#include "IInitializable.h"
#include <Windows.h> // todo: hate having to include Windows.h, maybe use a PIMPL pattern?

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

class TextureShader : public IInitializable
{
public:
	TextureShader();
	TextureShader(const TextureShader&) = delete;
	virtual ~TextureShader() override;

	void Initialize(ID3D11Device *pDevice, HWND hwnd);
	void Render(
		ID3D11DeviceContext *pContext,
		int x,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		ID3D11ShaderResourceView *);

	TextureShader& operator =(const TextureShader&) = delete;

private:
	void InitializeShader(
		ID3D11Device* pDevice,
		HWND hwnd,
		const std::string& vertexShaderFile,
		const std::string& pixelShaderFile );
	virtual void OnShutdown() override;
	void ShutdownShader();
	void SetShaderParameters(
		ID3D11DeviceContext *pContext,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		ID3D11ShaderResourceView *);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader *mpVertexShader;
	ID3D11PixelShader * mpPixelShader;
	ID3D11InputLayout * mpLayout;
	ID3D11Buffer * mpMatrixBuffer;
	ID3D11SamplerState * mpSamplerState;
};
