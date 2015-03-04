#pragma once

#include <memory>

// Forward declarations.
struct ID3D11Material;		// ?
struct ID3D11ShaderResourceView;

namespace DXSandboxApp
{
	class Texture2d;

    // Wrapper around DirectX texture 2d.
    class Material
    {
    public:
        Material();
        Material(
			std::shared_ptr<Texture2d> texture,
            ID3D11SamplerState * samplerState);
        ~Material();

        // Load texture.
        void Initialize(
			std::shared_ptr<Texture2d> texture,
            ID3D11SamplerState * samplerState);

        // Unload texture object.
        void Reset();

        // Check if the texture was initialized and is ready for display.
        bool IsValid() const;

        // Retrieve the ID3D11Texture2D texture pointer.
        ID3D11Texture2D * GetTexture();

        // Retrieve the ID3D11Texture2D texture pointer.
        const ID3D11Texture2D * GetTexture() const;

        // Retrieve the ID3D11ShaderResourceView pointer.
        ID3D11ShaderResourceView * GetShaderResourceView();

        // Retrieve the ID3D11ShaderResourceView pointer.
        const ID3D11ShaderResourceView * GetShaderResourceView() const;

        // Retrieve the ID3D11SamplerState texture pointer.
        ID3D11SamplerState * GetSamplerState();

        // Retrieve the ID3D11SamplerState texture pointer.
        const ID3D11SamplerState * GetSamplerState() const;

        void SetMeshColor(DirectX::XMFLOAT4 color);
        void SetDiffuseColor(DirectX::XMFLOAT4 color);
        void SetSpecularColor(DirectX::XMFLOAT4 color);
        void SetSpecularExponent(float value);

        DirectX::XMFLOAT4 MeshColor() const;
        DirectX::XMFLOAT4 DiffuseColor() const;
        DirectX::XMFLOAT4 SpecularColor() const;
        float SpecularExponent() const;

    private:
        bool mLoaded;
		std::shared_ptr<Texture2d> mTexture;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
        DirectX::XMFLOAT4 mMeshColor;
        DirectX::XMFLOAT4 mDiffuseColor;
        DirectX::XMFLOAT4 mSpecularColor;
        float mSpecularExponent;
    };
}
