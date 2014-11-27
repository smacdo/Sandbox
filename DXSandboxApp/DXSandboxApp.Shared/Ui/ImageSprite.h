#pragma once

#include <string>
#include <memory>

namespace DX
{
    class DeviceResources;
}

struct ID2DBitmap;

namespace DXSandboxApp
{
    class RenderableImageSprite;

    // Renders UI text.
    //  TODO: Consider splitting into two classes. One holds renderer calls (Create/Release) and instance is held by renderer. The
    //        other class is given to game code, and proxies requests into this class. That way game code doesn't see things like
    //        create/release device dependent resources.
    class ImageSprite
    {
    public:
        ImageSprite(std::shared_ptr<RenderableImageSprite> renderableSprite);
        ImageSprite(const ImageSprite&) = delete;
        ~ImageSprite();

        ImageSprite& operator = (const ImageSprite& rhs) = delete;

        void SetText(const std::wstring& text);
        void SetPosition(float x, float y);

        std::pair<float, float> Size() const;

    private:
        std::shared_ptr<RenderableImageSprite> mRenderable;
    };

    ///////////

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

