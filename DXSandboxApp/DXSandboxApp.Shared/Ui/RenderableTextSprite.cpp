#include "pch.h"
#include "RenderableTextSprite.h"

#include "Common/DirectXHelper.h"

using namespace DXSandboxApp;

// Constants
static const wchar_t * DefaultFontName = L"Segoe UI";
static const wchar_t * DefaultRegion = L"en-US";
static const int DefaultFontSize = 32;
static const float MaxDrawTextWidth = 800.0f;
static const float MaxDrawTextHeight = 100.0f;

RenderableTextSprite::RenderableTextSprite(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    const std::wstring& text)
    : RenderableTextSprite(deviceResources, text, DefaultFontName, DefaultFontSize)
{
}

RenderableTextSprite::RenderableTextSprite(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
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
        static_cast<float>(fontSize),
        DefaultRegion,
        &mFormat));

    DX::ThrowIfFailed(mFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
    DX::ThrowIfFailed(mFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));

    // Misc device depdenndent resources.
    DX::ThrowIfFailed(mDeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&mStateBlock));

    // Update text.
    SetText(text);
}

RenderableTextSprite::~RenderableTextSprite()
{
}

void RenderableTextSprite::SetText(const std::wstring& text)
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

void RenderableTextSprite::SetPosition(float x, float y)
{
    mPosX = x;
    mPosY = y;
}

std::pair<float, float> RenderableTextSprite::Size() const
{
    DWRITE_TEXT_METRICS	textMetrics = { 0 };
    DX::ThrowIfFailed(mLayout->GetMetrics(&textMetrics));

    return std::pair<float, float>(textMetrics.layoutWidth, textMetrics.height);
}


void RenderableTextSprite::CreateDeviceDependentResources()
{
    // TODO: Mark state as recreated
    DX::ThrowIfFailed(
        mDeviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &mBrush));
}

void RenderableTextSprite::ReleaseDeviceDependentResources()
{
    // TODO: Mark state as released
    mBrush.Reset();
}

void RenderableTextSprite::Render()
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