#include "UiTextRenderer.h"
#include "Font.h"
#include "FontShader.h"
#include "DXSandbox.h"
#include "Camera.h"
#include "Dx3d.h"
#include "DrawableText.h"
#include "Size.h"

#include "SimpleMath.h"
#include <d3d11.h>
#include <memory>

using namespace DirectX::SimpleMath;

UiTextRenderer::UiTextRenderer()
    : IInitializable(),
      mScreenSize(),
      mFont(),
      mFontShader(),
      mTexts(),
      mSentence1(),
      mSentence2()
{
}

UiTextRenderer::~UiTextRenderer()
{
}

void UiTextRenderer::Initialize(Dx3d& dx, const Size& screenSize)
{
    mScreenSize = screenSize;

    // Create font object and shader.
    mFont.reset(new Font());
    mFont->Initialize(dx.GetDevice(), L".\\Fonts\\rastertek.txt", L".\\Fonts\\rastertek.dds");

    mFontShader.reset(new FontShader());
    mFontShader->Initialize(dx);

    // Initialize sentences.
    mSentence1.reset(new DrawableText("name1"));
    mSentence1->Initialize(dx, 16);
    mSentence1->Update(dx, *mFont.get(), "Hello", screenSize, Vector2(100, 100), Color(1.0f, 1.0f, 1.0f));

    mSentence2.reset(new DrawableText("name2"));
    mSentence2->Initialize(dx, 16);
    mSentence2->Update(dx, *mFont.get(), "World", screenSize, Vector2(100, 200), Color(1.0f, 1.0f, 0.0f));
}

void UiTextRenderer::Add(
    Dx3d& dx,
    const std::string& name,
    const std::string& text,
    const DirectX::SimpleMath::Vector2& position,
    const DirectX::SimpleMath::Color& color)
{
    std::unique_ptr<DrawableText> drawableText(new DrawableText(name));
    drawableText->Initialize(dx, name.length());
    drawableText->Update(
        dx,
        *mFont.get(),
        text,
        mScreenSize,
        position,
        color);

    mTexts.push_back(std::move(drawableText));
}

void UiTextRenderer::OnShutdown()
{
}

void UiTextRenderer::Render(Dx3d& dx, const Camera& camera, const Matrix& worldMatrix)
{
    // Iterate through list of text objects to render.
    for (const auto& drawableText : mTexts)
    {
        drawableText->Render(dx, *mFont.get(), *mFontShader.get(), camera, worldMatrix);
    }

    mSentence1->Render(dx, *mFont.get(), *mFontShader.get(), camera, worldMatrix);
    mSentence2->Render(dx, *mFont.get(), *mFontShader.get(), camera, worldMatrix);
}