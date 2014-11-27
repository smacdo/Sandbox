#include "pch.h"
#include "GameUiRenderer.h"

#include "Common/DirectXHelper.h"
#include "Common/ResourceLoader.h"
#include "Ui/TextSprite.h"
#include "Ui/ImageSprite.h"

using namespace DXSandboxApp;

/**
 * Initializes D2D resources used for text rendering.
 */
GameUiRenderer::GameUiRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources)
    : mDeviceResources(deviceResources),
      mResourceLoader(new ResourceLoader(deviceResources)),
      mTextSprite(new TextSprite(deviceResources, L"Hello World")),
      mImageSprite(),
      mImageSprite2()
{
	CreateDeviceDependentResources();
}

/**
 * Destructor.
 */
GameUiRenderer::~GameUiRenderer()
{
}

/**
 * Update text to be displayed.
*/
void GameUiRenderer::Update(DX::StepTimer const& timer)
{
	// Update FPS text.
	uint32 fps = timer.GetFramesPerSecond();
	std::wstring textString = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

    mTextSprite->SetText(textString);
}

// Renders a frame to the screen.
void GameUiRenderer::Render()
{
    // Draw sprite objects as a big batch.
    auto d2dContext = mDeviceResources->GetD2DDeviceContext();
    d2dContext->BeginDraw();

    mImageSprite->Render();
    mImageSprite2->Render();

    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    // Draw text objects.
    mTextSprite->Render();
}

void GameUiRenderer::CreateDeviceDependentResources()
{
    mTextSprite->CreateDeviceDependentResources();

    // Load sprites (have to reload after device lost).
    mImageSprite.reset(mResourceLoader->LoadImageSprite(L"Logo.png"));
    mImageSprite2.reset(mResourceLoader->LoadImageSprite(L"StoreLogo.png"));

    mImageSprite2->SetPosition(300, 400);
}

void GameUiRenderer::ReleaseDeviceDependentResources()
{
    mTextSprite->ReleaseDeviceDependentResources();
    mImageSprite->ReleaseDeviceDependentResources();
    mImageSprite2->ReleaseDeviceDependentResources();
}

void GameUiRenderer::CreateWindowSizeDependentResources()
{
    // (Re-)Position text objects.
    ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();
    Windows::Foundation::Size logicalSize = mDeviceResources->GetLogicalSize();

    std::pair<float, float> size = mTextSprite->LayoutSize();
    mTextSprite->SetPosition(logicalSize.Width - size.first, logicalSize.Height - size.second);
}