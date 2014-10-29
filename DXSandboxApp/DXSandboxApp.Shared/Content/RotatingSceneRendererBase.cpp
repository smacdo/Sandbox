#include "pch.h"
#include "RotatingSceneRendererBase.h"
#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "Common\DirectXHelper.h"

#include <memory>

using namespace DXSandboxApp;

using namespace DirectX;
using namespace Windows::Foundation;

// Basic constructor.
RotatingSceneRendererBase::RotatingSceneRendererBase(std::shared_ptr<DX::DeviceResources> deviceResources)
    : mLoadingComplete(false),
      mDegreesPerSecond(45),
      mTracking(false),
      mDeviceResources(deviceResources)
{
    CreateModelViewBuffer();
    UpdateModelViewBuffer();
}

// Initializes view parameters when the window size changes.
void RotatingSceneRendererBase::UpdateModelViewBuffer()
{
    Size outputSize = mDeviceResources->GetOutputSize();
    float aspectRatio = outputSize.Width / outputSize.Height;
    float fovAngleY = 70.0f * XM_PI / 180.0f;

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
        100.0f
        );

    XMFLOAT4X4 orientation = mDeviceResources->GetOrientationTransform3D();
    XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

    XMStoreFloat4x4(&mModelViewBufferData.projection,XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

    // Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
    static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
    static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
    static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

    XMStoreFloat4x4(&mModelViewBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}


void RotatingSceneRendererBase::ReleaseDeviceDependentResources()
{
    ReleaseModelViewBuffer();
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void RotatingSceneRendererBase::Update(DX::StepTimer const& timer)
{
    if (!IsTracking())
    {
        // Convert degrees to radians, then convert seconds to rotation angle
        float radiansPerSecond = XMConvertToRadians(mDegreesPerSecond);
        double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
        float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

        RotateScene(radians);
    }
}

void RotatingSceneRendererBase::CreateWindowSizeDependentResources()
{
    UpdateModelViewBuffer();
}

// Rotate the 3D cube model a set amount of radians.
void RotatingSceneRendererBase::RotateScene(float radians)
{
    XMStoreFloat4x4(&mModelViewBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void RotatingSceneRendererBase::StartTracking()
{
    mTracking = true;
}

void RotatingSceneRendererBase::StopTracking()
{
    mTracking = false;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void RotatingSceneRendererBase::TrackingUpdate(float positionX)
{
    if (IsTracking())
    {
        float radians = XM_2PI * 2.0f * positionX / mDeviceResources->GetOutputSize().Width;
        RotateScene(radians);
    }
}

void RotatingSceneRendererBase::CreateModelViewBuffer()
{
    CD3D11_BUFFER_DESC bufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    HRESULT hr = mDeviceResources->GetD3DDevice()->CreateBuffer(&bufferDesc, nullptr, &mModelViewBuffer);
    DX::ThrowIfFailed(hr);
}

void RotatingSceneRendererBase::ReleaseModelViewBuffer()
{
    mLoadingComplete = false;
    mModelViewBuffer.Reset();
}

void RotatingSceneRendererBase::SetLoadingComplete(bool isLoadingComplete)
{
    mLoadingComplete = isLoadingComplete;
}