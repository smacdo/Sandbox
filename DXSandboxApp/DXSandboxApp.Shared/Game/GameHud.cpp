#include "pch.h"
#include "GameHud.h"
#include "Rendering/GameUiRenderer.h"
#include "Common/StepTimer.h"

#include "Ui\ImageSprite.h"
#include "Ui\TextSprite.h"

using namespace DXSandboxApp;

GameHud::GameHud(std::shared_ptr<GameUiRenderer> uiRenderer)
    : mUiRenderer(uiRenderer)
{    
}

GameHud::~GameHud()
{
}

void GameHud::LoadResources()
{
    mFpsStatus.reset(mUiRenderer->CreateTextSprite(L"? FPS"));
    mImage1.reset(mUiRenderer->CreateImageSprite(L"Logo.png"));
    mImage2.reset(mUiRenderer->CreateImageSprite(L"StoreLogo.png"));
}

void GameHud::UnloadResources()
{
    mFpsStatus.reset();
    mImage1.reset();
    mImage2.reset();
}

void GameHud::Update(const DX::StepTimer& timer)
{
    // Update FPS text.
    uint32 fps = timer.GetFramesPerSecond();
    std::wstring textString = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

    mFpsStatus->SetText(textString);
}

void GameHud::OnScreenSizeChanged(float width, float height)
{
    std::pair<float, float> size = mFpsStatus->Size();
    mFpsStatus->SetPosition(width - size.first, height - size.second);
}