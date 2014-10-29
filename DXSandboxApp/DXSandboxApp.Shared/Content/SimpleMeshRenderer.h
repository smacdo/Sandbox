#pragma once

#include <memory>

#include "Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "Common\StepTimer.h"
#include "RotatingSceneRendererBase.h"

struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

namespace DXSandboxApp
{
    // This sample renderer instantiates a basic rendering pipeline.
    class SimpleMeshRenderer : public RotatingSceneRendererBase
    {
    public:
        SimpleMeshRenderer(std::shared_ptr<DX::DeviceResources> deviceResources);
        void CreateDeviceDependentResources();
        virtual void ReleaseDeviceDependentResources() override;
        virtual void Update(const DX::StepTimer& timer) override;
        virtual void Render() override;

    private:

    private:
        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>	mInputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		mVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		mIndexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>	mVertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>	mPixelShader;

        // System resources for cube geometry.
        uint32	mIndexCount;
    };
}

