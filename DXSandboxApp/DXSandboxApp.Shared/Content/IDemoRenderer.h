#pragma once

#include <memory>

struct ID3D11Buffer;

namespace DX
{
    class DeviceResources;
    class StepTimer;
}

namespace DXSandboxApp
{
    // Interface definition for a demo renderer. All renderers must provide this functionality to be usable from the
    // WinRT application shell.
    class IDemoRenderer
    {
    public:
        IDemoRenderer() {};
        virtual ~IDemoRenderer() {};

        virtual void Render() = 0;
        virtual void Update(const DX::StepTimer& timer) = 0;

        virtual void CreateWindowSizeDependentResources() = 0;
        virtual void ReleaseDeviceDependentResources() = 0;
        virtual void CreateDeviceDependentResources() = 0;

        // TODO: Remove this, put it into separate class called "InputTracking" or wahtnot.
        virtual void StartTracking() = 0;
        virtual void StopTracking() = 0;
        virtual bool IsTracking() const = 0;
        virtual void TrackingUpdate(float positionX) = 0;
    };
}

