#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"

#include <memory>
#include <ppltasks.h>

// Renders Direct2D and 3D content on the screen.
namespace DXSandboxApp
{
    class DebugInfoRenderer;
    class IDemoRenderer;

	class DXSandboxAppMain : public DX::IDeviceNotify
	{
	public:
		DXSandboxAppMain(std::shared_ptr<DX::DeviceResources> deviceResources);
		~DXSandboxAppMain();
		void CreateWindowSizeDependentResources();
        void StartTracking();
        void TrackingUpdate(float positionX);
        void StopTracking();
        bool IsTracking();
		void StartRenderLoop();
		void StopRenderLoop();
		Concurrency::critical_section& GetCriticalSection() { return mCriticalSection; }

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
        void UpdateGameLoop();
		void ProcessInput();
		void Update();
		void Render();

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> mDeviceResources;

		// Scene renderers.
        std::unique_ptr<IDemoRenderer> mSceneRenderer;
		std::unique_ptr<DebugInfoRenderer> mDebugInfoRenderer;

        // Render details.
		Windows::Foundation::IAsyncAction^ mRenderLoopWorker;
		Concurrency::critical_section mCriticalSection;

		// Rendering loop timer.
        DX::StepTimer mUpdateTimer;
		DX::StepTimer mRenderTimer;

		// Track current input pointer position.
		float mPointerLocationX;
	};
}