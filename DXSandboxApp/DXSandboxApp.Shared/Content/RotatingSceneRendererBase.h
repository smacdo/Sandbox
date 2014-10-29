#pragma once

#include <memory>

#include "Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "Common\StepTimer.h"

struct ID3D11Buffer;

namespace DXSandboxApp
{
    // This is an abstract base for basic scene renderers. Controls the touch and camera for making demos simpler.
    //  TODO: Split this class up
    //     - BasicSceneRenderer
    //        - BasicRotatableScene
    class RotatingSceneRendererBase
    {
    public:
        RotatingSceneRendererBase(std::shared_ptr<DX::DeviceResources> deviceResources);
        
        virtual void Render() = 0;
        virtual void Update(const DX::StepTimer& timer);

        virtual void CreateWindowSizeDependentResources();
        virtual void ReleaseDeviceDependentResources();

        void StartTracking();
        void StopTracking();
        bool IsTracking() const { return mTracking; }

        bool IsLoadingComplete() const { return mLoadingComplete; }

        void TrackingUpdate(float positionX);

    protected:
        void SetLoadingComplete(bool isLoadingComplete);

    private:
        void CreateModelViewBuffer();
        void ReleaseModelViewBuffer();
        void UpdateModelViewBuffer();

        void RotateScene(float radians);

    protected:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> mDeviceResources;

        // Camera view data.
        Microsoft::WRL::ComPtr<ID3D11Buffer> mModelViewBuffer;
        ModelViewProjectionConstantBuffer	mModelViewBufferData;

    private:
        // Variables used with the rendering loop.
        bool	mLoadingComplete;
        float	mDegreesPerSecond;
        bool	mTracking;
    };
}

