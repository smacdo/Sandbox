#include "pch.h"
#include "RenderableImageSprite.h"

#include "Common/DirectXHelper.h"

using namespace DXSandboxApp;

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