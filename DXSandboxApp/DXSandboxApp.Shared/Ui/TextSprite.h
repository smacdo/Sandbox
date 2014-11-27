#pragma once

#include <string>
#include <memory>

namespace DXSandboxApp
{
    class RenderableTextSprite;

    // Sprite with text.
    class TextSprite
    {
    public:
        TextSprite(std::shared_ptr<RenderableTextSprite> renderable);
        TextSprite(const TextSprite&) = delete;
        ~TextSprite();

        TextSprite& operator = (const TextSprite& rhs) = delete;

        void SetText(const std::wstring& text);
        void SetPosition(float x, float y);

        std::pair<float, float> Size() const;

    private:
        std::shared_ptr<RenderableTextSprite> mRenderable;
    };
}

