#include "pch.h"
#include "SimpleMeshRenderer.h"
#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "Common\DirectXHelper.h"

#include <memory>

using namespace DXSandboxApp;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
SimpleMeshRenderer::SimpleMeshRenderer(std::shared_ptr<DX::DeviceResources> deviceResources)
    : RotatingSceneRendererBase(deviceResources),
      mIndexCount(0)
{
    CreateDeviceDependentResources();
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void SimpleMeshRenderer::Update(const DX::StepTimer& timer)
{
    RotatingSceneRendererBase::Update(timer);
}

// Renders one frame using the vertex and pixel shaders.
void SimpleMeshRenderer::Render()
{
    // Loading is asynchronous. Only draw geometry after it's loaded.
    if (!IsLoadingComplete())
    {
        return;
    }

    auto context = mDeviceResources->GetD3DDeviceContext();

    // Prepare the constant buffer to send it to the graphics device.
    context->UpdateSubresource(mModelViewBuffer.Get(), 0, NULL, &mModelViewBufferData, 0, 0);

    // Each vertex is one instance of the VertexPositionColor struct.
    unsigned int stride = sizeof(VertexPositionColor);
    unsigned int offset = 0;

    context->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset );
    context->IASetIndexBuffer(
        mIndexBuffer.Get(),
        DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
        0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(mInputLayout.Get());

    // Attach our vertex shader.
    context->VSSetShader(mVertexShader.Get(), nullptr, 0);

    // Send the constant buffer to the graphics device.
    context->VSSetConstantBuffers(0, 1, mModelViewBuffer.GetAddressOf());

    // Attach our pixel shader.
    context->PSSetShader(mPixelShader.Get(), nullptr, 0);

    // Draw the objects.
    context->DrawIndexed(mIndexCount, 0, 0);
}

void SimpleMeshRenderer::CreateDeviceDependentResources()
{
    // Load shaders asynchronously.
    auto loadVSTask = DX::ReadDataAsync(L"ColoredCubeVertexShader.cso");
    auto loadPSTask = DX::ReadDataAsync(L"ColoredCubePixelShader.cso");

    // After the vertex shader file is loaded, create the shader and input layout.
    auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateVertexShader(
                &fileData[0],
                fileData.size(),
                nullptr,
                &mVertexShader));

        static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateInputLayout(
                vertexDesc,
                ARRAYSIZE(vertexDesc),
                &fileData[0],
                fileData.size(),
                &mInputLayout));
    });

    // After the pixel shader file is loaded, create the shader and constant buffer.
    auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreatePixelShader(
                &fileData[0],
                fileData.size(),
                nullptr,
                &mPixelShader));
    });

    // Once both shaders are loaded, create the mesh.
    auto createCubeTask = (createPSTask && createVSTask).then([this]() {

        // Load mesh vertices. Each vertex has a position and a color.
        static const VertexPositionColor cubeVertices[] =
        {
            { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
            { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
            { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
            { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
            { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        };

        D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
        vertexBufferData.pSysMem = cubeVertices;
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &mVertexBuffer));

        // Load mesh indices. Each trio of indices represents a triangle to be rendered on the screen.
        // For example: 0,2,1 means that the vertices with indexes 0, 2 and 1 from the vertex buffer compose the first
        //              triangle of this mesh.
        static const unsigned short cubeIndices[] =
        {
            0, 2, 1, // -x
            1, 2, 3,

            4, 5, 6, // +x
            5, 7, 6,

            0, 1, 5, // -y
            0, 5, 4,

            2, 6, 7, // +y
            2, 7, 3,

            0, 4, 6, // -z
            0, 6, 2,

            1, 3, 7, // +z
            1, 7, 5,
        };

        mIndexCount = ARRAYSIZE(cubeIndices);

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = cubeIndices;
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
        DX::ThrowIfFailed(
            mDeviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &mIndexBuffer));
    });

    // Once the cube is loaded, the object is ready to be rendered.
    createCubeTask.then([this]() {
        SetLoadingComplete(true);
    });
}

void SimpleMeshRenderer::ReleaseDeviceDependentResources()
{
    RotatingSceneRendererBase::ReleaseDeviceDependentResources();

    mVertexShader.Reset();
    mInputLayout.Reset();
    mPixelShader.Reset();
    mVertexBuffer.Reset();
    mIndexBuffer.Reset();
}