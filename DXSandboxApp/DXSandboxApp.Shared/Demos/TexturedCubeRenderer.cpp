#include "pch.h"
#include "TexturedCubeRenderer.h"
#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "Common\DirectXHelper.h"
#include "Common\ModelViewConstantBuffer.h"
#include "Common\ResourceLoader.h"
#include "Input\InputTracker.h"

#include <memory>

using namespace DXSandboxApp;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
TexturedCubeRenderer::TexturedCubeRenderer(
    std::shared_ptr<InputTracker> inputTracker,
    std::shared_ptr<ResourceLoader> resourceLoader,
    std::shared_ptr<DX::DeviceResources> deviceResources)
    : BasicDemoRenderer(inputTracker, resourceLoader, deviceResources),
    mIndexCount(0)
{
    CreateDeviceDependentResources();
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void TexturedCubeRenderer::Update(const DX::StepTimer& timer)
{
    BasicDemoRenderer::Update(timer);
}

// Renders one frame using the vertex and pixel shaders.
void TexturedCubeRenderer::Render()
{
    BasicDemoRenderer::Render();

    // Loading is asynchronous. Only draw geometry after it's loaded.
    if (!IsLoadingComplete())
    {
        return;
    }

    auto context = mDeviceResources->GetD3DDeviceContext();

    // Each vertex is one instance of the VertexPositionColor struct.
    unsigned int stride = sizeof(VertexPositionColor);
    unsigned int offset = 0;

    context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(
        mIndexBuffer.Get(),
        DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
        0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(mInputLayout.Get());

    // Attach our vertex shader.
    context->VSSetShader(mVertexShader.Get(), nullptr, 0);

    // Send the MVP (model view projection) constant buffer to the graphics device.
    mModelViewBuffer->BindToActiveVertexShader(context, 0);

    // Attach our pixel shader.
    context->PSSetShader(mPixelShader.Get(), nullptr, 0);

    // Draw the objects.
    context->DrawIndexed(mIndexCount, 0, 0);
}

void TexturedCubeRenderer::CreateDeviceDependentResources()
{
    // Create and load cube vertex shader.
    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =        // TODO: Make separate function.
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    auto loadVSTask = mResourceLoader->LoadVertexShaderAndCreateInputLayoutAsync(
        L"SimpleModelVertexShader.cso",
        vertexDesc,
        ARRAYSIZE(vertexDesc));

    auto createVSTask = loadVSTask.then([this](std::tuple<ID3D11VertexShader*, ID3D11InputLayout*> results) {
        std::tie(mVertexShader, mInputLayout) = results;
    });

    // Create and load cube pixel shader.
    auto loadPSTask = mResourceLoader->LoadPixelShaderAsync(L"SimpleModelPixelShader.cso");
    auto createPSTask = loadPSTask.then([this](ID3D11PixelShader* pixelShader) {
        mPixelShader = pixelShader;
    });

    // Create the cube mesh once the vertex and pixel shaders have loaded.
    auto createCubeTask = (createPSTask && createVSTask).then([this]() {
        CreateCubeMesh(&mVertexBuffer, &mIndexBuffer, nullptr, &mIndexCount);
    });

    // Once the cube is loaded, the object is ready to be rendered.
    createCubeTask.then([this]() {
        SetLoadingComplete(true);
    });
}

void TexturedCubeRenderer::ReleaseDeviceDependentResources()
{
    BasicDemoRenderer::ReleaseDeviceDependentResources();

    mVertexShader.Reset();
    mInputLayout.Reset();
    mPixelShader.Reset();
    mVertexBuffer.Reset();
    mIndexBuffer.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility factory methods
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Generate a cube mesh in simple model format.
 */
void TexturedCubeRenderer::CreateCubeMesh(
    _Out_ ID3D11Buffer ** vertexBufferOut,
    _Out_ ID3D11Buffer ** indexBufferOut,
    _Out_opt_ unsigned int * vertexCountOut,
    _Out_opt_ unsigned int * indexCountOut)
{
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

    if (vertexCountOut != nullptr)
    {
        *vertexCountOut = ARRAYSIZE(cubeVertices);
    }

    // Initialize vertex buffer description and data structures, and then ask DirectX to create a new vertex buffer.
    CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);

    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    vertexBufferData.pSysMem = cubeVertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
        mDeviceResources->GetD3DDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            vertexBufferOut));

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

    if (indexCountOut != nullptr)
    {
        *indexCountOut = ARRAYSIZE(cubeIndices);
    }

    // Initialize index buffer description and data structures, and then ask DirectX to create a new index buffer.
    CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = cubeIndices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    
    DX::ThrowIfFailed(
        mDeviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            indexBufferOut));
}
