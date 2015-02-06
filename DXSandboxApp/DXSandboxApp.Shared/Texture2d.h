#pragma once

// Forward declarations.
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace DXSandboxApp
{
    // Wrapper around DirectX texture 2d.
    class Texture2d
    {
    public:
        Texture2d();
        Texture2d(ID3D11Texture2D * pTexture, ID3D11ShaderResourceView * pResourceView);
        ~Texture2d();

        // Load texture.
        void Initialize(ID3D11Texture2D * pTexture, ID3D11ShaderResourceView * pResourceView);

        // Unload texture object.
        void Reset();

        // Check if the texture was initialized and is ready for display.
        bool IsValid() const;

        // Retrieve the ID3D11Texture2D texture pointer.
        ID3D11Texture2D * GetTexture2D();

        // Retrieve the ID3D11Texture2D texture pointer.
        ID3D11ShaderResourceView * GetShaderResourceView();

    private:
        bool mLoaded;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
    };
}
