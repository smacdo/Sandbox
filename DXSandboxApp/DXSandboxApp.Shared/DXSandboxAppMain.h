#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"

#include <memory>
#include <ppltasks.h>

// Renders Direct2D and 3D content on the screen.
namespace DXSandboxApp
{
    class GameUiRenderer;
    class IDemoRenderer;
    class InputTracker;
    class GameHud;

	class DXSandboxAppMain : public DX::IDeviceNotify
	{
	public:
		DXSandboxAppMain(std::shared_ptr<DX::DeviceResources> deviceResources);
		~DXSandboxAppMain();
		void CreateWindowSizeDependentResources();
		void StartRenderLoop();
		void StopRenderLoop();
		Concurrency::critical_section& GetCriticalSection() { return mCriticalSection; }

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

        void Initialize();
        void StartRenderer(IDemoRenderer * renderer);

		std::shared_ptr<InputTracker> GetInputTracker();

	private:
        void UpdateGameLoop();
		void ProcessInput();
		void Update();
		void Render();

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> mDeviceResources;
        std::shared_ptr<InputTracker> mInputTracker;

		// Scene renderers.
        std::unique_ptr<IDemoRenderer> mSceneRenderer;
        std::shared_ptr<GameUiRenderer> mUiRenderer;

        // Render details.
		Windows::Foundation::IAsyncAction^ mRenderLoopWorker;
		Concurrency::critical_section mCriticalSection;

		// Rendering loop timer.
        DX::StepTimer mUpdateTimer;
		DX::StepTimer mRenderTimer;

        // Gameplay
        std::unique_ptr<GameHud> mGameHud;
	};
}