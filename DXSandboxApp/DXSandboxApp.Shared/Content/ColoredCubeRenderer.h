#pragma once

#include <memory>

#include "Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "IDemoRenderer.h"

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
    class ColoredCubeRenderer : public IDemoRenderer
	{
	public:
        ColoredCubeRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		virtual void CreateDeviceDependentResources() override;
		virtual void CreateWindowSizeDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(DX::StepTimer const& timer) override;
		virtual void Render() override;
		virtual void StartTracking() override;
		virtual void TrackingUpdate(float positionX) override;
		virtual void StopTracking() override;
		virtual bool IsTracking() const override { return m_tracking; }

	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

