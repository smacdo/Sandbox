#pragma once

#include <memory>

#include "Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "BasicDemoRenderer.h"

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
	// This is a simple demonstration of C++cx DirectX provided by Microsoft, and modified by Scott.
    // This class is responsible for rendering a simple rainbow color shaded cube.
    class ColoredCubeRenderer : public BasicDemoRenderer
	{
	public:
        ColoredCubeRenderer(std::shared_ptr<DX::DeviceResources> deviceResources);
        virtual void CreateDeviceDependentResources() override;
        virtual void ReleaseDeviceDependentResources() override;
        virtual void Update(const DX::StepTimer& timer) override;
        virtual void Render() override;

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

