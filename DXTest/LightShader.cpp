#include "LightShader.h"
#include "BinaryBlob.h"
#include "DXSandbox.h"
#include "SimpleMath.h"
#include "DXTestException.h"
#include "Light.h"
#include "Camera.h"
#include "ConstantBufferUpdater.h"

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

#include <d3d11.h>

using namespace DirectX::SimpleMath;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Light shader constants
///////////////////////////////////////////////////////////////////////////////////////////////////
static const wchar_t LightVertexShaderFilePath[] = L".\\Shaders\\SimpleLightVertexShader.cso";
static const wchar_t LightPixelShaderFilePath[] = L".\\Shaders\\SimpleLightPixelShader.cso";

struct matrix_buffer_t
{
    Matrix world;
    Matrix view;
    Matrix projection;
};

struct camera_buffer_t
{
    Vector3 cameraPosition;
    float padding;
};

struct light_buffer_t
{
    Vector4 ambientColor;
    Vector4 diffuseColor;
    Vector3 lightDirection;
    float specularPower;
    Vector4 specularColor;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Light shader implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
LightShader::LightShader()
    : IInitializable(),
      mVertexShader(),
      mPixelShader(),
      mLayout(),
      mMatrixBuffer(),
      mCameraBuffer(),
      mSamplerState(),
      mLightBuffer()
{
}

LightShader::~LightShader()
{
}

void LightShader::Initialize(Dx3d& dx)
{
    if (!IsInitialized())
    {
        HRESULT hr = InitializeShader(dx);
        VerifyDXResult(hr);

        SetInitialized();
    }
}

// break this into two functions LoadPixelShader, LoadVertexShader
// change WCHAR to std::wstring or std::string and convert last second
HRESULT LightShader::InitializeShader(Dx3d& dx)
{
    HRESULT hr = S_OK;

    // Load and create the pixel and vertex shaders.
    {
        BinaryBlob vertexShaderBlob = BinaryBlob::LoadFromFile(LightVertexShaderFilePath);
        BinaryBlob pixelShaderBlob = BinaryBlob::LoadFromFile(LightPixelShaderFilePath);

        hr = dx.CreateVertexShader(vertexShaderBlob, &mVertexShader);

        if (SUCCEEDED(hr))
        {
            hr = dx.CreatePixelShader(pixelShaderBlob, &mPixelShader);
        }

        // Now that we vertex shader is created, we can create the vertex input layout.
        if (SUCCEEDED(hr))
        {
            hr = CreateInputLayout(dx, vertexShaderBlob, &mLayout);
        }
    }

    // Create constant buffers for MVP camera projection, camera position, and lighting information.
    if (SUCCEEDED(hr))
    {
        hr = dx.CreateConstantBuffer(sizeof(matrix_buffer_t), &mMatrixBuffer);
    }
    
    if (SUCCEEDED(hr))
    {
        hr = dx.CreateConstantBuffer(sizeof(camera_buffer_t), &mCameraBuffer);
    }

    if (SUCCEEDED(hr))
    {
        hr = dx.CreateConstantBuffer(sizeof(light_buffer_t), &mLightBuffer);
    }

    // Create a texture sampler state description.
    if (SUCCEEDED(hr))
    {
        hr = dx.CreateTextureSamplerState(&mSamplerState);
    }

    return hr;
}

HRESULT LightShader::CreateInputLayout(
    Dx3d& dx,
    const BinaryBlob& vertexShaderBlob,
    ID3D11InputLayout **ppLayoutOut) const
{
    VerifyNotNull(ppLayoutOut);
    *ppLayoutOut = nullptr;

    // Describe the layout of data that will be fed to this shader.
    // This layout needs to match the vertex type structure defined in the model class and shader.
    const size_t INPUT_ELEMENT_COUNT = 3;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[INPUT_ELEMENT_COUNT];

    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    // Create the vertex input.
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

    HRESULT hr = dx.GetDevice()->CreateInputLayout(
        polygonLayout,
        INPUT_ELEMENT_COUNT,
        vertexShaderBlob.BufferPointer(),
        static_cast<SIZE_T>(vertexShaderBlob.BufferSize()),
        &inputLayout);

    if (SUCCEEDED(hr))
    {
        *ppLayoutOut = inputLayout.Detach();
    }

    return hr;
}

void LightShader::Render(
    Dx3d& dx,
    ID3D11DeviceContext *pDeviceContext,
    int indexCount,
    const Matrix& worldMatrix,
    const Matrix& viewMatrix,
    const Matrix& projectionMatrix,
    ID3D11ShaderResourceView *pTexture,
    const Camera& camera,
    const Light& light)
{
    if (!IsInitialized()) { throw NotInitializedException(L"LightShader"); }
    VerifyNotNull(pDeviceContext);
    VerifyNotNull(pTexture);

    SetShaderParameters(
        dx,
        pDeviceContext,
        worldMatrix,
        viewMatrix,
        projectionMatrix,
        pTexture,
        camera,
        light);

    RenderShader(pDeviceContext, indexCount);
}

void LightShader::SetShaderParameters(
    Dx3d& dx,
    ID3D11DeviceContext *pDeviceContext,
    const Matrix& inWorldMatrix,
    const Matrix& inViewMatrix,
    const Matrix& inProjectionMatrix,
    ID3D11ShaderResourceView *pTexture,
    const Camera& camera,
    const Light& light)
{
    AssertNotNull(pDeviceContext);
    HRESULT hr = S_OK;

    // Update the camera matrices in the MVP constants buffer.
    // Transpose the matrices to prepare them for the shader. This is a requirement for DirectX 11.
    hr = UpdateConstantsBuffer<matrix_buffer_t, ShaderType::Vertex, 0>(
        dx.GetDeviceContext(),
        mMatrixBuffer.Get(),
        [&](matrix_buffer_t& m) {
            m.world = inWorldMatrix.Transpose();
            m.view = inViewMatrix.Transpose();
            m.projection = inProjectionMatrix.Transpose();
    });

    VerifyDXResult(hr);

    // Update camera position constant buffer with new camera positioning information.
    hr = UpdateConstantsBuffer<camera_buffer_t, ShaderType::Vertex, 1>(
        dx.GetDeviceContext(),
        mCameraBuffer.Get(),
        [&](camera_buffer_t& c) {
            c.cameraPosition = camera.Position();
            c.padding = 0.0f;
    });

    VerifyDXResult(hr);

    // Update pixel shader with lighting information.
    hr = UpdateConstantsBuffer<light_buffer_t, ShaderType::Pixel, 0>(
        dx.GetDeviceContext(),
        mLightBuffer.Get(),
        [&](light_buffer_t& l) {
            l.ambientColor = light.AmbientColor();
            l.diffuseColor = light.DiffuseColor();
            l.lightDirection = light.Direction();
            l.specularPower = light.SpecularPower();
            l.specularColor = light.SpecularColor();
    });

    VerifyDXResult(hr);

    // Set up shader texture resource in the pixel shader.
    pDeviceContext->PSSetShaderResources(0, 1, &pTexture);
}

void LightShader::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount)
{
    AssertNotNull(pDeviceContext);

    // Set the vertex input layout.
    pDeviceContext->IASetInputLayout(mLayout.Get());

    // Set the color vertex and pixel shader.
    pDeviceContext->VSSetShader(mVertexShader.Get(), NULL, 0);
    pDeviceContext->PSSetShader(mPixelShader.Get(), NULL, 0);

    // Set the texture sampler state in the pixel shader.
    ID3D11SamplerState* samplerStates[1] = { mSamplerState.Get() };

    pDeviceContext->PSSetSamplers(0, 1, samplerStates);

    // Render the object.
    pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

void LightShader::OnShutdown()
{
}