#pragma once

#include <memory>

#include "Common/DeviceResources.h"
#include "Rendering/ShaderStructures.h"
#include "Rendering/BasicDemoRenderer.h"

struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

namespace DX
{
    class StepTimer;
}

namespace DXSandboxApp
{
    class InputTracker;
    class Texture2d;

    // Modified ColoredCubeRenderer that uses texture mapping.
    class TexturedCubeRenderer : public BasicDemoRenderer
    {
    public:
        TexturedCubeRenderer(
            std::shared_ptr<InputTracker> inputTracker,
            std::shared_ptr<ResourceLoader> resourceLoader,
            std::shared_ptr<DX::DeviceResources> deviceResources);
        virtual void CreateDeviceDependentResources() override;
        virtual void ReleaseDeviceDependentResources() override;
        virtual void Update(const DX::StepTimer& timer) override;
        virtual void Render() override;

    private:
        // TODO: Move this into a factory.
        void CreateCubeMesh(
            _Out_ ID3D11Buffer ** vertexBufferOut,
            _Out_ ID3D11Buffer ** indexBufferOut,
            _Out_opt_ unsigned int * vertexCountOut,
            _Out_opt_ unsigned int * indexCountOut);

    private:
        std::unique_ptr<Texture2d> mCubeTexture;

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

