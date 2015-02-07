#include "pch.h"
#include "BasicDemoRenderer.h"
#include "IDemoRenderer.h"
#include "Input\InputTracker.h"
#include "Common\ModelViewConstantBuffer.h"
#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "Common\DirectXHelper.h"
#include "Common\Camera.h"

#include <memory>

using namespace DXSandboxApp;

using namespace DirectX;
using namespace Windows::Foundation;

// Basic constructor.
BasicDemoRenderer::BasicDemoRenderer(
    std::shared_ptr<InputTracker> inputTracker,
    std::shared_ptr<ResourceLoader> resourceLoader,
    std::shared_ptr<DX::DeviceResources> deviceResources)
    : IDemoRenderer(),
      mCamera(new Camera()),
      mInputTracker(inputTracker),
      mLoadingComplete(false),
      mDegreesPerSecond(45),
      mTracking(false),
      mDeviceResources(deviceResources),
      mResourceLoader(resourceLoader)
{
    Initialize();
}

BasicDemoRenderer::~BasicDemoRenderer()
{
}

void BasicDemoRenderer::Initialize()
{
    // Configure initial camera view.
    mCamera->SetProjParams(XM_PI / 2, 1.0f, 0.01f, 100.0f);
    mCamera->SetViewParams(
        XMFLOAT3(0.0f, 0.0f, -5.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 1.0f, 0.0f));

    // Create and set up the constant view buffer.
    CreateModelViewBuffer();
    UpdateModelViewBuffer();
}

void BasicDemoRenderer::CreateModelViewBuffer()
{
    mModelViewBuffer.reset(new ModelViewConstantBuffer(mDeviceResources->GetD3DDevice()));
}

void BasicDemoRenderer::ReleaseModelViewBuffer()
{
    mLoadingComplete = false;
    mModelViewBuffer.reset();
}

// Initializes view parameters when the window size changes.
void BasicDemoRenderer::UpdateModelViewBuffer()
{
    Size outputSize = mDeviceResources->GetOutputSize();
    float aspectRatio = outputSize.Width / outputSize.Height;
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    mCamera->SetProjParams(
        XM_PI / 2,
        outputSize.Width / outputSize.Height,
        0.01f,
        1000.0f);

    // This is a simple example of change that can be made when the app is in portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // Note that the OrientationTransform3D matrix is post-multiplied here in order to correctly orient the scene to
    // match the display orientation. This post-multiplication step is required for any draw calls that are made to the
    // swap chain render target. For draw calls to other targets, this transform should not be applied.

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f);

    auto orientation = mDeviceResources->GetOrientationTransform3D();
    XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

    mModelViewBuffer->SetProjection(perspectiveMatrix * orientationMatrix);

    // Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
    static const XMVECTORF32 eye = { 0.0f, 0.0f, -5.0f, 0.0f };
    static const XMVECTORF32 at = { 0.0f,  0.0f, 0.0f, 0.0f };
    static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

    mModelViewBuffer->SetView(XMMatrixLookAtRH(eye, at, up));
}

void BasicDemoRenderer::ReleaseDeviceDependentResources()
{
    ReleaseModelViewBuffer();
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void BasicDemoRenderer::Update(DX::StepTimer const& timer)
{
    if (!mInputTracker->IsTracking())
    {
        // Convert degrees to radians, then convert seconds to rotation angle
        float radiansPerSecond = XMConvertToRadians(mDegreesPerSecond);
        double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
        float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

        RotateScene(radians);
    }
}

void BasicDemoRenderer::CreateWindowSizeDependentResources()
{
    UpdateModelViewBuffer();
}

// Rotate the 3D cube model a set amount of radians.
void BasicDemoRenderer::RotateScene(float radians)
{
    mModelViewBuffer->SetModel(XMMatrixRotationY(radians));
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void BasicDemoRenderer::Render()
{
    // Loading is asynchronous. Only draw geometry after it's loaded.
    if (!IsLoadingComplete())
    {
        return;
    }

    // Update rotation based on input tracking.
    if (mInputTracker->IsTracking())
    {
        float radians = XM_2PI * 2.0f * mInputTracker->InputPositionX() / mDeviceResources->GetOutputSize().Width;
        RotateScene(radians);
    }

    // Update the model-view-projection constant buffer with current MVP values, and bind it for shaders to access.
    auto context = mDeviceResources->GetD3DDeviceContext();
    mModelViewBuffer->ApplyChanges(context);
}

void BasicDemoRenderer::SetLoadingComplete(bool isLoadingComplete)
{
    mLoadingComplete = isLoadingComplete;
}