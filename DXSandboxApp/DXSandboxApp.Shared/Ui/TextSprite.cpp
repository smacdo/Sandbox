#include "pch.h"
#include "TextSprite.h"

#include "Common/DirectXHelper.h"

using namespace DXSandboxApp;

// Constants
static const wchar_t * DefaultFontName = L"Segoe UI";
static const wchar_t * DefaultRegion = L"en-US";
static const float DefaultFontSize = 32.0f;
static const float MaxDrawTextWidth = 800.0f;
static const float MaxDrawTextHeight = 100.0f;

TextSprite::TextSprite(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::wstring& text)
    : TextSprite(deviceResources, text, DefaultFontName, DefaultFontSize)
{
}

TextSprite::TextSprite(const std::shared_ptr<DX::DeviceResources>& deviceResources,
                       const std::wstring& text,
                       const std::wstring& fontName,
                       int fontSize)
    : mText(),
      mPosX(0.0f),
      mPosY(0.0f),
      mDeviceResources(deviceResources),
      mBrush(),
      mStateBlock(),
      mLayout(),
      mFormat()
{
    // Query for text format information for the selected font.
    DX::ThrowIfFailed(
        mDeviceResources->GetDWriteFactory()->CreateTextFormat(
        fontName.c_str(),
        nullptr,
        DWRITE_FONT_WEIGHT_LIGHT,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        DefaultRegion,
        &mFormat));

    DX::ThrowIfFailed(mFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
    DX::ThrowIfFailed(mFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));

    // Misc device depdenndent resources.
    DX::ThrowIfFailed(mDeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&mStateBlock));

    // Update text.
    SetText(text);
}

TextSprite::~TextSprite()
{
}

void TextSprite::SetText(const std::wstring& text)
{
    mText = text;

    DX::ThrowIfFailed(
        mDeviceResources->GetDWriteFactory()->CreateTextLayout(
        mText.c_str(),
        static_cast<uint32>(mText.length()),
        mFormat.Get(),
        MaxDrawTextWidth,  // Max width of the input text.
        MaxDrawTextHeight, // Max height of the input text.
        &mLayout));
}

void TextSprite::SetPosition(float x, float y)
{
    mPosX = x;
    mPosY = y;
}

std::pair<float, float> TextSprite::LayoutSize() const
{
    DWRITE_TEXT_METRICS	textMetrics = { 0 };
    DX::ThrowIfFailed(mLayout->GetMetrics(&textMetrics));

    return std::pair<float, float>(textMetrics.layoutWidth, textMetrics.height);
}


void TextSprite::CreateDeviceDependentResources()
{
    // TODO: Mark state as recreated
    DX::ThrowIfFailed(
        mDeviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &mBrush));
}

void TextSprite::ReleaseDeviceDependentResources()
{
    // TODO: Mark state as released
    mBrush.Reset();
}

void TextSprite::Render()
{
    // Draw text.
    ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();

    context->SaveDrawingState(mStateBlock.Get());
    context->BeginDraw();

    D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(mPosX, mPosY);
    context->SetTransform(screenTranslation * mDeviceResources->GetOrientationTransform2D());

    context->DrawTextLayout(
        D2D1::Point2F(0.f, 0.f),
        mLayout.Get(),
        mBrush.Get());

    // Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device is lost. It will be handled during the
    // next call to Present.
    HRESULT hr = context->EndDraw();

    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    context->RestoreDrawingState(mStateBlock.Get());
}