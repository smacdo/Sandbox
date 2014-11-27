#include "pch.h"
#include "ImageSprite.h"

#include "Common/DirectXHelper.h"

using namespace DXSandboxApp;

// Constants
static const wchar_t * DefaultFontName = L"Segoe UI";
static const wchar_t * DefaultRegion = L"en-US";
static const float DefaultFontSize = 32.0f;
static const float MaxDrawTextWidth = 800.0f;
static const float MaxDrawTextHeight = 100.0f;

ImageSprite::ImageSprite(std::shared_ptr<RenderableImageSprite> renderableSprite)
    : mRenderable(renderableSprite)
{
}

ImageSprite::~ImageSprite()
{
}

void ImageSprite::SetPosition(float x, float y)
{
    mRenderable->SetPosition(x, y);
}

std::pair<float, float> ImageSprite::Size() const
{
    return mRenderable->Size();
}

////////////////////////////////////////////////////////////////////////////////////////////
RenderableImageSprite::RenderableImageSprite(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    ID2D1Bitmap * spriteBitmap,     // Take ownership with AddRef.
    float spriteWidth,
    float spriteHeight)
    : mDeviceResources(deviceResources),
      mBitmap(spriteBitmap),
      mWidth(spriteWidth),
      mHeight(spriteHeight),
      mPosX(0.0f),
      mPosY(0.0f)
{
}

RenderableImageSprite::~RenderableImageSprite()
{
}

void RenderableImageSprite::SetPosition(float x, float y)
{
    mPosX = x;
    mPosY = y;
}

std::pair<float, float> RenderableImageSprite::Size() const
{
    return std::pair<float, float>(mPosX, mPosY);
}


void RenderableImageSprite::CreateDeviceDependentResources()
{
    // TODO: Mark state as recreated
}

void RenderableImageSprite::ReleaseDeviceDependentResources()
{
    // TODO: Mark state as released
}

void RenderableImageSprite::Render()
{
    auto d2dContext = mDeviceResources->GetD2DDeviceContext();

    d2dContext->DrawBitmap(
        mBitmap.Get(),
        D2D1::RectF(mPosX, mPosY, mPosX + mWidth, mPosY + mHeight));
}