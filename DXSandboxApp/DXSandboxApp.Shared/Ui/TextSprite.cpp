#include "pch.h"
#include "TextSprite.h"
#include "Ui/RenderableTextSprite.h"

using namespace DXSandboxApp;

TextSprite::TextSprite(std::shared_ptr<RenderableTextSprite> renderable)
    : mRenderable(renderable)
{
}

TextSprite::~TextSprite()
{
}

void TextSprite::SetText(const std::wstring& text)
{
    mRenderable->SetText(text);
}

void TextSprite::SetPosition(float x, float y)
{
    mRenderable->SetPosition(x, y);
}

std::pair<float, float> TextSprite::Size() const
{
    return mRenderable->Size();
}