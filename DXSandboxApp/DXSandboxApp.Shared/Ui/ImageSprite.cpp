#include "pch.h"
#include "ImageSprite.h"
#include "Ui/RenderableImageSprite.h"

using namespace DXSandboxApp;

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