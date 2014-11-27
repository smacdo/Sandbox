#pragma once

#include <string>
#include <memory>
#include <vector>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

struct DWRITE_TEXT_METRICS;
struct ID2D1SolidColorBrush;
struct ID2D1DrawingStateBlock;
struct IDWriteTextLayout;
struct IDWriteTextFormat;

namespace DXSandboxApp
{
    class TextSprite;
    class RenderableImageSprite;
    class RenderableTextSprite;
    class ImageSprite;
    class ResourceLoader;

	// Renders the game UI.
    //  TODO: Split this class into two parts. First part is the UI renderer, which should just render and expose
    //        methods like "CreateTextSprite, CreateImageSprite, RemoveSprite, etc" and a second gameplay part that
    //        doesn't worry about rendering and things.
	class GameUiRenderer
	{
	public:
        GameUiRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~GameUiRenderer();
		void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

        TextSprite * CreateTextSprite(const std::wstring& text);
        ImageSprite * CreateImageSprite(const std::wstring& imageFilePath);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> mDeviceResources;
        std::unique_ptr<ResourceLoader> mResourceLoader;

        // Text rendering.
        std::vector<std::shared_ptr<RenderableTextSprite>> mTextSprites;
        std::vector<std::shared_ptr<RenderableImageSprite>> mImageSprites;
	};
}