#include "pch.h"
#include "DXSandboxAppMain.h"
#include "Common/DirectXHelper.h"
#include "Rendering/IDemoRenderer.h"
#include "Rendering/GameUiRenderer.h"
#include "Input/InputTracker.h"
#include "Game/GameHud.h"
#include "Demos/ColoredCubeRenderer.h"

#include <memory>

using namespace DXSandboxApp;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

/**
 * Loads and initializes application assets when the application is loaded.
 */
DXSandboxAppMain::DXSandboxAppMain(std::shared_ptr<DX::DeviceResources> deviceResources)
    : mDeviceResources(deviceResources),
      mInputTracker(new InputTracker()),
      mPointerLocationX(0.0f)
{
	// Register to be notified if the Device is lost or recreated
	mDeviceResources->RegisterDeviceNotify(this);

    Initialize();
    StartRenderer(new ColoredCubeRenderer(mInputTracker, mDeviceResources));
}

/**
 * Destructor cleanup.
 */
DXSandboxAppMain::~DXSandboxAppMain()
{
	// Deregister device notification
	mDeviceResources->RegisterDeviceNotify(nullptr);
}

void DXSandboxAppMain::Initialize()
{
    // Initialize system / game timer to fixed updates, 60 times a second.
    mUpdateTimer.SetFixedTimeStep(false);

    mRenderTimer.SetFixedTimeStep(true);
    mRenderTimer.SetTargetElapsedSeconds(1.0 / 60);

    // Initialize the user interface.
    mUiRenderer = std::unique_ptr<GameUiRenderer>(new GameUiRenderer(mDeviceResources));

    mGameHud.reset(new GameHud(mUiRenderer));
    mGameHud->LoadResources();
}

void DXSandboxAppMain::StartRenderer(IDemoRenderer * renderer)
{
    critical_section::scoped_lock lock(mCriticalSection);
    mSceneRenderer = std::unique_ptr<IDemoRenderer>(renderer);
}

// Updates application state when the window size changes (e.g. device orientation change)
void DXSandboxAppMain::CreateWindowSizeDependentResources() 
{
	mSceneRenderer->CreateWindowSizeDependentResources();
    mUiRenderer->CreateWindowSizeDependentResources();

    // Resize the game ui.
    std::pair<float, float> screenSize = mUiRenderer->LogicalSize();
    mGameHud->OnScreenSizeChanged(screenSize.first, screenSize.second);
}

void DXSandboxAppMain::StartRenderLoop()
{
	// If the animation render loop is already running then do not start another thread.
	if (mRenderLoopWorker != nullptr && mRenderLoopWorker->Status == AsyncStatus::Started)
	{
		return;
	}

	// Create a task that will be run on a background thread.
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		// Calculate the updated frame and render once per vertical blanking interval.
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(mCriticalSection);
            UpdateGameLoop();
		}
	});

	// Run task on a dedicated high priority background thread.
	mRenderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void DXSandboxAppMain::StopRenderLoop()
{
    mRenderLoopWorker->Cancel();
}

void DXSandboxAppMain::UpdateGameLoop()
{
    // Get user input and process that before updating, ensuring all input is immediately applied prior to a rendering
    // call.
    ProcessInput();

    // Update game state before rendering it. This call will execute as few or as many update calls as needed to ensure
    // the game state is synchronized to game time.
    Update();

    // Allow game a chance to render a frame of output reflecting the current game state.
    if (mUpdateTimer.GetFrameCount() != 0)  // Don't render before the first Update.
    {
        Render();
        mDeviceResources->Present();
    }
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
void DXSandboxAppMain::Render()
{
    auto context = mDeviceResources->GetD3DDeviceContext();

    // Reset the viewport to target the whole screen.
    auto viewport = mDeviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    // Reset render targets to the screen.
    ID3D11RenderTargetView *const targets[1] = { mDeviceResources->GetBackBufferRenderTargetView() };
    context->OMSetRenderTargets(1, targets, mDeviceResources->GetDepthStencilView());

    // Clear the back buffer and depth stencil view.
    context->ClearRenderTargetView(mDeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
    context->ClearDepthStencilView(mDeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Render active demo scene, if one was selected.
    if (mSceneRenderer)
    {
        mSceneRenderer->Render();
    }

    // Render the user interface.
    mUiRenderer->Render();
}

// Updates the application state once per frame.
void DXSandboxAppMain::Update() 
{
	// Update scene objects. Call the update lambda as few or as many times as needed to ensure we are caught up to the
    // current game time. Logic for doing this is embedded in StepTimer::Update() function.
	mUpdateTimer.Update([&](double totalTime, double deltaTime)
	{
        mGameHud->Update(mUpdateTimer);

        // Update active demo renderer, if one is running.
        if (mSceneRenderer)
        {
            mSceneRenderer->Update(mUpdateTimer);
        }

        // Update the user interface.
        mUiRenderer->Update(mUpdateTimer);
	});
}

// Process all input from the user before updating game state
void DXSandboxAppMain::ProcessInput()
{
    mInputTracker->TrackingUpdate(mPointerLocationX);
}

// Notifies renderers that device resources need to be released.
void DXSandboxAppMain::OnDeviceLost()
{
    if (mSceneRenderer)
    {
        mSceneRenderer->ReleaseDeviceDependentResources();
    }

	mUiRenderer->ReleaseDeviceDependentResources();    
    mGameHud->UnloadResources();
}

// Notifies renderers that device resources may now be recreated.
void DXSandboxAppMain::OnDeviceRestored()
{
    if (mSceneRenderer)
    {
        mSceneRenderer->CreateDeviceDependentResources();
    }

    mUiRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

    mGameHud->LoadResources();
}

void DXSandboxAppMain::StartTracking()
{
    // TODO: Move this code to the caller.
    mInputTracker->StartTracking();
}

void DXSandboxAppMain::TrackingUpdate(float positionX)
{
    // TODO: Move this code to the caller.
    mInputTracker->TrackingUpdate(positionX);
    mPointerLocationX = positionX;
}

void DXSandboxAppMain::StopTracking()
{
    // TODO: Move this code to the caller.
    mInputTracker->StopTracking();
}

bool DXSandboxAppMain::IsTracking()
{
    // TODO: Move this code to the caller.
    return mInputTracker->IsTracking();
}