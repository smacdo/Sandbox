#pragma once

#include <string>
#include <memory>

namespace DXSandboxApp
{
    class ImageSprite;

    class ResourceLoader
    {
    public:
        ResourceLoader(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ResourceLoader(const ResourceLoader&) = delete;
        ~ResourceLoader();

        ResourceLoader& operator = (const ResourceLoader&) = delete;

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

        ImageSprite* LoadImageSprite(const std::wstring& imageFileName);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> mDeviceResources;
    };
}