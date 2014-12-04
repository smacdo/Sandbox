#pragma once

#include <memory>

#include "IDemoRenderer.h"
#include "Common/DeviceResources.h"
#include "Rendering/ShaderStructures.h"

struct ID3D11Buffer;

namespace DX
{
    class StepTimer;
}

namespace DXSandboxApp
{
    class InputTracker;
    class ModelViewConstantBuffer;
    class ResourceLoader;

    // This is an abstract base for basic scene renderers. Controls the touch and camera for making demos simpler.
    //  TODO: Split this class up
    //     - BasicSceneRenderer
    //        - BasicRotatableScene
    class BasicDemoRenderer : public IDemoRenderer
    {
    public:
        BasicDemoRenderer(
            std::shared_ptr<InputTracker> inputTracker,
            std::shared_ptr<ResourceLoader> resourceLoader,
            std::shared_ptr<DX::DeviceResources> deviceResources);
        virtual ~BasicDemoRenderer();
        
        virtual void Render();
        virtual void Update(const DX::StepTimer& timer);

        virtual void CreateWindowSizeDependentResources();
        virtual void ReleaseDeviceDependentResources();

        bool IsLoadingComplete() const { return mLoadingComplete; }

    protected:
        void SetLoadingComplete(bool isLoadingComplete);

    private:
        void CreateModelViewBuffer();
        void ReleaseModelViewBuffer();
        void UpdateModelViewBuffer();

        void RotateScene(float radians);

    protected:
        // Pointer to input tracking.
        std::shared_ptr<InputTracker> mInputTracker;

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> mDeviceResources;

        // Cached pointer to resource loader.
        std::shared_ptr<ResourceLoader> mResourceLoader;

        // Camera view data.
        std::shared_ptr<ModelViewConstantBuffer> mModelViewBuffer;

    private:
        // Variables used with the rendering loop.
        bool	mLoadingComplete;
        float	mDegreesPerSecond;
        bool	mTracking;
    };
}

