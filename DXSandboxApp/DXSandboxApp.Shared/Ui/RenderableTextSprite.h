#pragma once

#include <string>
#include <memory>

struct ID2D1SolidColorBrush;
struct ID2D1DrawingStateBlock;
struct IDWriteTextLayout;
struct IDWriteTextFormat;

namespace DXSandboxApp
{
    // Internal render class.
    class RenderableTextSprite
    {
    public:
        RenderableTextSprite(
            const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::wstring& text);

        RenderableTextSprite(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            const std::wstring& text,
            const std::wstring& fontName,
            int fontSize);

        RenderableTextSprite(const RenderableTextSprite&) = delete;
        ~RenderableTextSprite();

        RenderableTextSprite& operator = (const RenderableTextSprite& rhs) = delete;

        void SetText(const std::wstring& text);
        void SetPosition(float x, float y);
        std::pair<float, float> Size() const;

        void Render();

        // Called by renderer when device is recreated.
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

    private:
        std::wstring mText;
        float mPosX, mPosY;
        std::shared_ptr<DX::DeviceResources> mDeviceResources;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> mBrush;
        Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> mStateBlock;
        Microsoft::WRL::ComPtr<IDWriteTextLayout> mLayout;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> mFormat;
    };
}

