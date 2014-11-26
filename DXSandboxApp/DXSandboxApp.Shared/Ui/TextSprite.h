#pragma once

#include <string>
#include <memory>

struct ID2D1SolidColorBrush;
struct ID2D1DrawingStateBlock;
struct IDWriteTextLayout;
struct IDWriteTextFormat;

namespace DXSandboxApp
{
    // Renders UI text.
    //  TODO: Consider splitting into two classes. One holds renderer calls (Create/Release) and instance is held by renderer. The
    //        other class is given to game code, and proxies requests into this class. That way game code doesn't see things like
    //        create/release device dependent resources.
    class TextSprite
    {
    public:
        TextSprite(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::wstring& text);

        TextSprite(const std::shared_ptr<DX::DeviceResources>& deviceResources,
                   const std::wstring& text,
                   const std::wstring& fontName,
                   int fontSize);

        TextSprite(const TextSprite&) = delete;
        ~TextSprite();

        TextSprite& operator = (const TextSprite& rhs) = delete;

        void SetText(const std::wstring& text);
        void SetPosition(float x, float y);
        std::pair<float, float> LayoutSize() const;

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

