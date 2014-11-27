#pragma once

#include <string>
#include <memory>

namespace DXSandboxApp
{
    class RenderableImageSprite;

    // A renderable image sprite.
    class ImageSprite
    {
    public:
        ImageSprite(std::shared_ptr<RenderableImageSprite> renderableSprite);
        ImageSprite(const ImageSprite&) = delete;
        ~ImageSprite();

        ImageSprite& operator = (const ImageSprite& rhs) = delete;

        void SetText(const std::wstring& text);
        void SetPosition(float x, float y);

        std::pair<float, float> Size() const;

    private:
        std::shared_ptr<RenderableImageSprite> mRenderable;
    };
}

