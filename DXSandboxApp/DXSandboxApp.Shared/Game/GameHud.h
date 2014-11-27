#pragma once

#include <string>
#include <memory>

namespace DX
{
    class StepTimer;
}

namespace DXSandboxApp
{
    class GameUiRenderer;
    class TextSprite;
    class ImageSprite;

    // Game user interface.
    class GameHud
    {
    public:
        GameHud(std::shared_ptr<GameUiRenderer> uiRenderer);
        GameHud(const GameHud&) = delete;
        ~GameHud();

        GameHud& operator = (const GameHud& rhs) = delete;

        void LoadResources();           // TODO: Rename LoadContent()
        void UnloadResources();

        void Update(const DX::StepTimer& timer);

    private:
        std::shared_ptr<GameUiRenderer> mUiRenderer;
        std::unique_ptr<TextSprite> mFpsStatus;
        std::unique_ptr<ImageSprite> mImage1;
        std::unique_ptr<ImageSprite> mImage2;
    };
}

