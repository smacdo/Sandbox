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
      mTextSprites(),
      mImageSprites()
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
}

// Renders a frame to the screen.
void GameUiRenderer::Render()
{
    // Draw sprite objects as a big batch.
    auto d2dContext = mDeviceResources->GetD2DDeviceContext();
    d2dContext->BeginDraw();

    for (auto &sprite : mImageSprites)
    {
        sprite->Render();
    }

    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    // Draw text objects.
    for (auto &sprite : mTextSprites)
    {
        sprite->Render();
    }
}

void GameUiRenderer::CreateDeviceDependentResources()
{
    // Text sprites.
    for (auto &sprite : mTextSprites)
    {
        sprite->CreateDeviceDependentResources();
    }

    // Image sprites.
    for (auto &sprite : mImageSprites)
    {
        sprite->CreateDeviceDependentResources();
    }
}

void GameUiRenderer::ReleaseDeviceDependentResources()
{
    // Text sprites.
    for (auto &sprite : mTextSprites)
    {
        sprite->ReleaseDeviceDependentResources();
    }

    // Image sprites.
    for (auto &sprite : mImageSprites)
    {
        sprite->ReleaseDeviceDependentResources();
    }
}

void GameUiRenderer::CreateWindowSizeDependentResources()
{
}

std::pair<float, float> GameUiRenderer::LogicalSize() const
{
    ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();
    Windows::Foundation::Size logicalSize = mDeviceResources->GetLogicalSize();

    return std::pair<float, float>(logicalSize.Width, logicalSize.Height);
}

TextSprite* GameUiRenderer::CreateTextSprite(const std::wstring& text)
{
    std::shared_ptr<RenderableTextSprite> renderable(new RenderableTextSprite(mDeviceResources, text));
    renderable->CreateDeviceDependentResources();

    mTextSprites.push_back(renderable);     // TODO: Presize array or something.
    return new TextSprite(renderable);
}

ImageSprite* GameUiRenderer::CreateImageSprite(const std::wstring& imageFilePath)
{
    std::shared_ptr<RenderableImageSprite> renderable(mResourceLoader->LoadImageSprite(imageFilePath)); // TODO: Rename LoadImageSprite
    mImageSprites.push_back(renderable);     // TODO: Presize array or something.

    return new ImageSprite(renderable);
}