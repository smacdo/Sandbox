#pragma once
#include "SimpleMath.h"
#include "IInitializable.h"

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

// TODO: A lot of these values can be set as parameters since they will not vary frame to frame.
//  -> font texture
//  -> pixel color.
// etc.
// TODO: Add ability to change pixsel color on the fly.
class FontShader : public IInitializable
{
public:
	FontShader();
	FontShader(const FontShader&) = delete;
	virtual ~FontShader() override;

	void Initialize(ID3D11Device *pDevice);
	void Render(
		ID3D11DeviceContext *pContext,
		int indexCount,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		ID3D11ShaderResourceView * pTexture,
        const DirectX::SimpleMath::Vector4& pixelColor);

	FontShader& operator =(const FontShader&) = delete;

private:
    void InitializeShader(ID3D11Device* pDevice);
	virtual void OnShutdown() override;
	void SetShaderParameters(
		ID3D11DeviceContext *pContext,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		ID3D11ShaderResourceView * pTexture,
        const DirectX::SimpleMath::Vector4& pixelColor);
	void RenderShader(ID3D11DeviceContext*, int);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mMatrixBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mPixelBuffer;
};
