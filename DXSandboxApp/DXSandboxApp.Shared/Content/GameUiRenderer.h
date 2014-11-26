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

	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
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

        // Text rendering.
        std::unique_ptr<DXSandboxApp::TextSprite> mTextSprite;
	};
}