#pragma once

#include "IInitializable.h"
#include "SimpleMath.h"
#include <string>
#include <memory>

class Font;
class FontShader;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
class Dx3d;
class DrawableText;

// This class is so terrible, it hurts.
// TODO: This is not a "text" class, it is a manager of text objects.
// TODO: Change sentence_t to text_t and this class to UiTextManager.
class UiTextRenderer : public IInitializable
{
public:
    UiTextRenderer();
    UiTextRenderer(const UiTextRenderer& text) = delete;
    virtual ~UiTextRenderer() override;

    UiTextRenderer& operator = (const UiTextRenderer& rhs) = delete;

    void Initialize(Dx3d& dx,
                    int screenWidth,
                    int screenHeight,
                    const DirectX::SimpleMath::Matrix& baseViewMatrix);

    void Render(Dx3d& dx,
                const DirectX::SimpleMath::Matrix& worldMatrix,
                const DirectX::SimpleMath::Matrix& orthoMatrix); //const;


private:
    virtual void OnShutdown() override;

private:
    std::unique_ptr<Font> mFont;
    std::unique_ptr<FontShader> mFontShader;

    int mScreenWidth;
    int mScreenHeight;
    DirectX::SimpleMath::Matrix mBaseViewMatrix;

    std::unique_ptr<DrawableText> mSentence1;
    std::unique_ptr<DrawableText> mSentence2;
};
