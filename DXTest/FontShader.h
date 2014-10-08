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
class Dx3d;
class BinaryBlob;
class Font;

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

    FontShader& operator =(const FontShader&) = delete;

    void Initialize(Dx3d& dx);

	void Render(
        Dx3d& dx,
        const Font& font,
		int indexCount,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
        const DirectX::SimpleMath::Vector4& pixelColor);

protected:
    virtual void OnShutdown() override;

private:
    HRESULT InitializeShader(Dx3d& dx);

    HRESULT CreateInputLayout(
        Dx3d& dx,
        const BinaryBlob& vertexShaderBlob,
        ID3D11InputLayout **ppLayoutOut) const;
	
	void SetShaderParameters(
		Dx3d& dx,
        const Font& font,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
        const DirectX::SimpleMath::Vector4& pixelColor);
	void RenderShader(Dx3d& dx, int);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mMatrixBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mPixelBuffer;          // TODO: terrible name
};
