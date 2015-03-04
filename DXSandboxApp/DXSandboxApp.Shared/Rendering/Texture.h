#pragma once

// Forward declarations.
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace DXSandboxApp
{
	class Texture2d
	{
	public:
		Texture2d();
		Texture2d(const std::wstring& name,
				  ID3D11Texture2D * pTexture,
				  ID3D11ShaderResourceView * pShaderResourceView);
		~Texture2d();
		
		void Initialize(
			const std::wstring& name,
			ID3D11Texture2D * pTexture,
			ID3D11ShaderResourceView * pShaderResourceView);

		std::wstring Name() const;
		ID3D11Texture2D * GetTexture();
		ID3D11Texture2D * GetTexture() const;
		ID3D11ShaderResourceView * GetShaderResourceView();
		ID3D11ShaderResourceView * GetShaderResourceView() const;

	private:
		std::wstring mName;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
	};
}


