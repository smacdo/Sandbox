#pragma once

#include "IInitializable.h"
#include "SimpleMath.h"
#include "Size.h"
#include <string>
#include <memory>

class Font;
class FontShader;
class Dx3d;
class DrawableText;
class Camera;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;


class UiTextRenderer : public IInitializable
{
public:
    UiTextRenderer();
    UiTextRenderer(const UiTextRenderer& text) = delete;
    virtual ~UiTextRenderer() override;

    UiTextRenderer& operator = (const UiTextRenderer& rhs) = delete;

    void Initialize(Dx3d& dx, const Size& screenSize);

    void Render(Dx3d& dx,
                const Camera& camera,
                const DirectX::SimpleMath::Matrix& worldMatrix);    // TODO: Const

private:
    virtual void OnShutdown() override;

private:
    std::unique_ptr<Font> mFont;
    std::unique_ptr<FontShader> mFontShader;

    std::unique_ptr<DrawableText> mSentence1;
    std::unique_ptr<DrawableText> mSentence2;
};
