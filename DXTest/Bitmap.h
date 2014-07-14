#pragma once

#include "IInitializable.h"
#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
class Texture;

// Handles 2d images.
//  TODO: Rename this class.
//  TODO: mPreviousX/Y is used to track if buffers need to be updated. Remove these values, use a dirty flag.
//  TODO: Honestly this class is really terrible. Just burn it and rewrite it from scratch.
class Bitmap : IInitializable
{
public:
    Bitmap();
    Bitmap(const Bitmap& bitmap) = delete;
    ~Bitmap();

    Bitmap& operator =(const Bitmap& rhs) = delete;

    void Initialize(ID3D11Device * pDevice,
                    int screenWidth,
                    int screenHeight,
                    const std::string& textureFile,
                    int bitmapWidth,
                    int bitmapHeight);
    void Render(ID3D11DeviceContext *pDeviceContext, int x, int y);
    int GetIndexCount() const;
    ID3D11ShaderResourceView * GetTexture() const;

protected:
    virtual void OnShutdown() override;

private:
    void InitializeBuffers(ID3D11Device *pDevice);
    void ShutdownBuffers();
    void UpdateBuffers(ID3D11DeviceContext *pDeviceContext, int x, int y);
    void RenderBuffers(ID3D11DeviceContext *pDeviceContext);
    void LoadTexture(ID3D11Device *pDevice, const std::string& textureFile);
    void ReleaseTexture();

private:
    ID3D11Buffer *mpVertexBuffer;
    ID3D11Buffer *mpIndexBuffer;
    int mVertexCount;
    int mIndexCount;
    Texture * mpTexture;

    // TODO: Do we really need to store screen dimensions?
    int mScreenWidth, mScreenHeight;
    int mBitmapWidth, mBitmapHeight;

    // Track previous position of this bitmap. If changed we need to update dynamic buffers.
    //  TODO: This is stupid.
    int mPreviousX, mPreviousY;
};

