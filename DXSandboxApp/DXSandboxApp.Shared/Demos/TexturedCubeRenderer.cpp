#include "pch.h"
#include "TexturedCubeRenderer.h"
#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "Common\DirectXHelper.h"
#include "Rendering\ConstantBuffers.h"
#include "Common\ResourceLoader.h"
#include "Input\InputTracker.h"
#include "Rendering/Material.h"
#include "Rendering/ConfigurableDesc.h"

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
      mCubeTexture(),
      mIndexCount(0),
      mRasterizerState(deviceResources->CreateRasterizerState())
{
    CreateDeviceDependentResources();
    deviceResources->GetD3DDeviceContext()->RSSetState(mRasterizerState.Get());
}

void TexturedCubeRenderer::CreateDeviceDependentResources()
{
    // Create and load cube vertex shader.
    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =        // TODO: Make separate function.
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    auto loadVSTask = mResourceLoader->LoadVertexShaderAndCreateInputLayoutAsync(
        L"TexturedCubeVertexShader.cso",
        vertexDesc,
        ARRAYSIZE(vertexDesc));

    auto createVSTask = loadVSTask.then([this](std::tuple<ID3D11VertexShader*, ID3D11InputLayout*> results) {
        std::tie(mVertexShader, mInputLayout) = results;
    });

    // Create and load cube pixel shader.
    auto loadPSTask = mResourceLoader->LoadPixelShaderAsync(L"TexturedCubePixelShader.cso");
    auto createPSTask = loadPSTask.then([this](ID3D11PixelShader* pixelShader) {
        mPixelShader = pixelShader;
    });

    // Create the cube mesh once the vertex and pixel shaders have loaded.
    auto createCubeTask = (createPSTask && createVSTask).then([this]() {
        CreateCubeMesh(&mVertexBuffer, &mIndexBuffer, nullptr, &mIndexCount);
    });

    // Load the texture map for the cube.
    SamplerSettings settings;
    settings = settings.AnisotropicFilter().MaxAnisotropy(8);

    auto loadTextureTask = mResourceLoader->LoadTexture2dAsync(L"crate.png", settings).then([this](Material * pTexture)
    {
        mCubeTexture.reset(pTexture);

        // Configure material values.
        mCubeTexture->SetMeshColor(XMFLOAT4(0.8f, 0.8f, 0.0f, 0.5f));
        mCubeTexture->SetDiffuseColor(XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f));
        mCubeTexture->SetSpecularColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
        mCubeTexture->SetSpecularExponent(5.0f);
    });

    // Once the cube is loaded, the object is ready to be rendered.
    (createCubeTask && loadTextureTask).then([this]() {
        SetLoadingComplete(true);
    });

    // Initialize the scene's constant buffer that holds lighting data.
    auto d3dDevice = mDeviceResources->GetD3DDevice();
    mPerFrameConstants.reset(new PerFrameConstantBuffer(d3dDevice));

    mPerFrameConstants->SetAmbientColor(XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f));
    mPerFrameConstants->SetDiffuseColor(XMFLOAT4(0.25f, 0.25f, 0.85f, 1.0f));

    auto d3dContext = mDeviceResources->GetD3DDeviceContext();
    mPerFrameConstants->ApplyChanges(d3dContext);

    // Initialize per primitive constant buffer.
    mPerPrimitiveConstants.reset(new PerPrimitiveConstantBuffer(d3dDevice));
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

    // Each vertex is one instance of the VertexSimple struct.
    unsigned int stride = sizeof(VertexSimple);
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

    // Send data to our constant buffers before rendering.
    BindConstantBuffers(context);

    // Bind the texture resource view and sampler state.
    ID3D11ShaderResourceView* shaderResourceViews[] = { mCubeTexture->GetShaderResourceView() };
    context->PSSetShaderResources(0, 1, shaderResourceViews);

    ID3D11SamplerState* samplerStates[] = { mCubeTexture->GetSamplerState() };
    context->PSSetSamplers(0, 1, samplerStates);

    // Attach our pixel shader.
    context->PSSetShader(mPixelShader.Get(), nullptr, 0);

    // Draw the objects.
    context->DrawIndexed(mIndexCount, 0, 0);
}

void TexturedCubeRenderer::BindConstantBuffers(_In_ ID3D11DeviceContext1 * pContext)
{
    // Upload per primitive values to the per primtiive constant buffer.
    mPerPrimitiveConstants->SetWorldMatrix(XMMatrixRotationY(mRotationAmount));
    mPerPrimitiveConstants->SetMeshColor(mCubeTexture->MeshColor());
    mPerPrimitiveConstants->SetDiffuseColor(mCubeTexture->DiffuseColor());
    mPerPrimitiveConstants->SetSpecularColor(mCubeTexture->SpecularColor());
    mPerPrimitiveConstants->SetSpecularExponent(mCubeTexture->SpecularExponent());

    mPerPrimitiveConstants->ApplyChanges(pContext);

    // Apply constant buffers.
    mPerFrameConstants->BindToActivePixelShader(pContext, 0);
    mModelViewBuffer->BindToActiveVertexShader(pContext, 1);
    mPerPrimitiveConstants->BindToActiveVertexShader(pContext, 2);

    mModelViewBuffer->BindToActivePixelShader(pContext, 1);
    mPerPrimitiveConstants->BindToActivePixelShader(pContext, 2);
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
    static const VertexSimple cubeVertices[] =
    {
        // Front Face
        VertexSimple(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
        VertexSimple(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f),
        VertexSimple(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f),
        VertexSimple(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

        // Back Face
        VertexSimple(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f),
        VertexSimple(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
        VertexSimple(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f),
        VertexSimple(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f),

        // Top Face
        VertexSimple(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f),
        VertexSimple(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
        VertexSimple(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
        VertexSimple(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),

        // Bottom Face
        VertexSimple(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),
        VertexSimple(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
        VertexSimple(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f),
        VertexSimple(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f),

        // Left Face
        VertexSimple(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f),
        VertexSimple(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
        VertexSimple(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, -1.0f),
        VertexSimple(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),

        // Right Face
        VertexSimple(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
        VertexSimple(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f),
        VertexSimple(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
        VertexSimple(1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f),
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
        // Front Face
        0, 1, 2,
        0, 2, 3,

        // Back Face
        4, 5, 6,
        4, 6, 7,

        // Top Face
        8, 9, 10,
        8, 10, 11,

        // Bottom Face
        12, 13, 14,
        12, 14, 15,

        // Left Face
        16, 17, 18,
        16, 18, 19,

        // Right Face
        20, 21, 22,
        20, 22, 23
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
