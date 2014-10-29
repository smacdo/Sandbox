#include "pch.h"
#include "DebugInfoRenderer.h"

#include "Common/DirectXHelper.h"

using namespace DXSandboxApp;

// Constants
static const wchar_t * DefaultFontName = L"Segoe UI";
static const wchar_t * DefaultRegion = L"en-US";
static const float DefaultFontSize = 32.0f;
static const float MaxDrawTextWidth = 240.0f;
static const float MaxDrawTextHeight = 50.0f;

/**
 * Initializes D2D resources used for text rendering.
 */
DebugInfoRenderer::DebugInfoRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources)
    : mText(L""),
      mDeviceResources(deviceResources)
{
	// Query for text format information for the selected font.
	DX::ThrowIfFailed(
		mDeviceResources->GetDWriteFactory()->CreateTextFormat(
            DefaultFontName,
			nullptr,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
            DefaultFontSize,
            DefaultRegion,
			&mTextFormat));

	DX::ThrowIfFailed(mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

    // Misc device depdenndent resources.
	DX::ThrowIfFailed(mDeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&mStateBlock));
	CreateDeviceDependentResources();
}

/**
 * Update text to be displayed.
*/
void DebugInfoRenderer::Update(DX::StepTimer const& timer)
{
	// Update display text.
	uint32 fps = timer.GetFramesPerSecond();

	mText = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

	DX::ThrowIfFailed(
		mDeviceResources->GetDWriteFactory()->CreateTextLayout(
			mText.c_str(),
			static_cast<uint32>(mText.length()),
			mTextFormat.Get(),
            MaxDrawTextWidth,  // Max width of the input text.
            MaxDrawTextHeight, // Max height of the input text.
			&mTextLayout));
}

// Renders a frame to the screen.
void DebugInfoRenderer::Render()
{
    // Get text metrics for the default text format.
    DWRITE_TEXT_METRICS	textMetrics = { 0 };
    DX::ThrowIfFailed(mTextLayout->GetMetrics(&textMetrics));

    // Draw text.
	ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = mDeviceResources->GetLogicalSize();

	context->SaveDrawingState(mStateBlock.Get());
	context->BeginDraw();

	// Position on the bottom right corner
	D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
		logicalSize.Width - textMetrics.layoutWidth,
		logicalSize.Height - textMetrics.height
		);

	context->SetTransform(screenTranslation * mDeviceResources->GetOrientationTransform2D());

	DX::ThrowIfFailed(mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));

	context->DrawTextLayout(
		D2D1::Point2F(0.f, 0.f),
		mTextLayout.Get(),
		mWhiteBrush.Get());

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device is lost. It will be handled during the
    // next call to Present.
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	context->RestoreDrawingState(mStateBlock.Get());
}

void DebugInfoRenderer::CreateDeviceDependentResources()
{
	DX::ThrowIfFailed(
		mDeviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &mWhiteBrush)
		);
}

void DebugInfoRenderer::ReleaseDeviceDependentResources()
{
	mWhiteBrush.Reset();
}