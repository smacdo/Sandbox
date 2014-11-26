#include "pch.h"
#include "GameUiRenderer.h"

#include "Common/DirectXHelper.h"
#include "Ui/TextSprite.h"

using namespace DXSandboxApp;

/**
 * Initializes D2D resources used for text rendering.
 */
GameUiRenderer::GameUiRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources)
    : mTextSprite(new TextSprite(deviceResources, L"Hello World")),
      mDeviceResources(deviceResources)
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
    mTextSprite->Render();
}

void GameUiRenderer::CreateDeviceDependentResources()
{
    mTextSprite->CreateDeviceDependentResources();
}

void GameUiRenderer::ReleaseDeviceDependentResources()
{
    mTextSprite->ReleaseDeviceDependentResources();
}

void GameUiRenderer::CreateWindowSizeDependentResources()
{
    // (Re-)Position text objects.
    ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();
    Windows::Foundation::Size logicalSize = mDeviceResources->GetLogicalSize();

    std::pair<float, float> size = mTextSprite->LayoutSize();
    mTextSprite->SetPosition(logicalSize.Width - size.first, logicalSize.Height - size.second);
}