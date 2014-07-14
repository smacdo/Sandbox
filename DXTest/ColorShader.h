#pragma once
#include <d3d11.h>
#include "SimpleMath.h"

class ColorShader
{
public:
	ColorShader();
	ColorShader(const ColorShader&) = delete;
	virtual ~ColorShader();

	void Initialize(ID3D11Device *pDevice, HWND hwnd);
	void Shutdown();
	void Render(
		ID3D11DeviceContext *pContext,
		int x,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&);

	ColorShader& operator =(const ColorShader&) = delete;

private:
	void InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	void OutputShaderErrorMessasge(ID3D10Blob *, HWND, WCHAR*);
	void SetShaderParameters(
		ID3D11DeviceContext *pContext,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&,
		const DirectX::SimpleMath::Matrix&);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D10Blob * LoadCompiledShaderFromFile(const std::string& filename) const;

private:
	bool mInitialized;
	ID3D11VertexShader *mpVertexShader;
	ID3D11PixelShader * mpPixelShader;
	ID3D11InputLayout * mpLayout;
	ID3D11Buffer * mpMatrixBuffer;
};

