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
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class DebugInfoRenderer
	{
	public:
        DebugInfoRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> mDeviceResources;

		// Resources related to text rendering.
		std::wstring                                    mText;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    mWhiteBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  mStateBlock;
		Microsoft::WRL::ComPtr<IDWriteTextLayout>       mTextLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat>		mTextFormat;
	};
}