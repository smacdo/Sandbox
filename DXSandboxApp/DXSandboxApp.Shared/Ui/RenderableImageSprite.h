#pragma once

#include <memory>

struct ID2DBitmap;

namespace DX
{
    class DeviceResources;
}

namespace DXSandboxApp
{
    // Contains data and logic for rendering an image sprite.
    class RenderableImageSprite
    {
    public:
        RenderableImageSprite(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            ID2D1Bitmap * spriteBitmap,          // ImageSprite takes ownership using AddRef.
            float spriteWidth,
            float spriteHeight);

        RenderableImageSprite(const RenderableImageSprite&) = delete;
        ~RenderableImageSprite();

        RenderableImageSprite& operator = (const RenderableImageSprite& rhs) = delete;

        void SetText(const std::wstring& text);
        void SetPosition(float x, float y);

        std::pair<float, float> Size() const;

        void Render();

        // Called by renderer when device is recreated.
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

    private:
        std::shared_ptr<DX::DeviceResources> mDeviceResources;
        Microsoft::WRL::ComPtr<ID2D1Bitmap> mBitmap;
        float mWidth, mHeight;
        float mPosX, mPosY;
    };
}

