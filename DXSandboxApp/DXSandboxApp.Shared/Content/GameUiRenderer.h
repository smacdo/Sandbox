#pragma once

#include <string>
#include <memory>
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

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> mDeviceResources;
        std::unique_ptr<ResourceLoader> mResourceLoader;

        // Text rendering.
        std::unique_ptr<DXSandboxApp::TextSprite> mTextSprite;
        std::unique_ptr<DXSandboxApp::ImageSprite> mImageSprite;
        std::unique_ptr<DXSandboxApp::ImageSprite> mImageSprite2;
	};
}