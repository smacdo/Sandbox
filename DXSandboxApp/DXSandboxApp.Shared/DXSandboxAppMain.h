﻿#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"

#include <memory>
#include <ppltasks.h>

// Renders Direct2D and 3D content on the screen.
namespace DXSandboxApp
{
    class SimpleMeshRenderer;
    class DebugInfoRenderer;

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
		void ProcessInput();
		void Update();
		bool Render();

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> mDeviceResources;

		// Scene renderers.
        std::unique_ptr<SimpleMeshRenderer> mSceneRenderer;     // tODO: Move to ISceneRenderer.
		std::unique_ptr<DebugInfoRenderer> mDebugInfoRenderer;

        // Render details.
		Windows::Foundation::IAsyncAction^ mRenderLoopWorker;
		Concurrency::critical_section mCriticalSection;

		// Rendering loop timer.
		DX::StepTimer mTimer;

		// Track current input pointer position.
		float mPointerLocationX;
	};
}