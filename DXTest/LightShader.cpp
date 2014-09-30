#include "LightShader.h"
#include "BinaryBlob.h"
#include "DXSandbox.h"
#include "SimpleMath.h"
#include "Light.h"
#include "Camera.h"

#include <d3d11.h>

using namespace DirectX::SimpleMath;

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

LightShader::LightShader()
: IInitializable(),
  mpVertexShader(nullptr),
  mpPixelShader(nullptr),
  mpLayout(nullptr),
  mpMatrixBuffer(nullptr),
  mpCameraBuffer(nullptr),
  mpSamplerState(nullptr),
  mpLightBuffer(nullptr)
{
}

LightShader::~LightShader()
{
}

void LightShader::Initialize(ID3D11Device *pDevice)
{
    VerifyNotNull(pDevice);

    if (!IsInitialized())
    {
        InitializeShader(pDevice, ".\\Shaders\\SimpleLightVertexShader.cso", ".\\Shaders\\SimpleLightPixelShader.cso");
        SetInitialized();
    }
}

// break this into two functions LoadPixelShader, LoadVertexShader
// change WCHAR to std::wstring or std::string and convert last second
void LightShader::InitializeShader(
    ID3D11Device *pDevice,
    const std::string& vertexShaderFile,
    const std::string& pixelShaderFile)
{
    // Load the pixel and vertex shaders.
    BinaryBlob vertexShaderBlob = BinaryBlob::LoadFromFile(vertexShaderFile);
    BinaryBlob pixelShaderBlob = BinaryBlob::LoadFromFile(pixelShaderFile);

    // Create the vertex shader object.
    HRESULT result;

    result = pDevice->CreateVertexShader(
        vertexShaderBlob.BufferPointer(),
        static_cast<SIZE_T>(vertexShaderBlob.BufferSize()),
        NULL,
        &mpVertexShader);

    VerifyDXResult(result);
    VerifyNotNull(mpVertexShader);

    // Create the pixel shader object.
    result = pDevice->CreatePixelShader(
        pixelShaderBlob.BufferPointer(),
        static_cast<SIZE_T>(pixelShaderBlob.BufferSize()),
        NULL,
        &mpPixelShader);

    VerifyDXResult(result);
    VerifyNotNull(mpPixelShader);

    // Describe the layout of data that will be fed to this shader.
    // This layout needs to match the vertex type structure defined in the model class and shader.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

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

    // Number of elements in the layout
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);	// XXX: FIXME TODO!!!

    // Create the vertex input.
    result = pDevice->CreateInputLayout(
        polygonLayout,
        numElements,
        vertexShaderBlob.BufferPointer(),
        static_cast<SIZE_T>(vertexShaderBlob.BufferSize()),
        &mpLayout);

    VerifyDXResult(result);
    VerifyNotNull(mpLayout);

    // Describe the dynamic matrix constant buffer that will be fed to the vertex shader.
    D3D11_BUFFER_DESC matrixBufferDesc;

    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(matrix_buffer_t);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the matrix buffer.
    result = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &mpMatrixBuffer);

    VerifyDXResult(result);
    VerifyNotNull(mpMatrixBuffer);

    // Describe a dynamic constant bufffer for the camera.
    D3D11_BUFFER_DESC cameraBufferDesc;

    cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    cameraBufferDesc.ByteWidth = sizeof(camera_buffer_t);
    cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cameraBufferDesc.MiscFlags = 0;
    cameraBufferDesc.StructureByteStride = 0;

    // Create the camera buffer.
    result = pDevice->CreateBuffer(&cameraBufferDesc, NULL, &mpCameraBuffer);

    VerifyDXResult(result);
    VerifyNotNull(mpCameraBuffer);

    // Create a texture sampler state description.
    D3D11_SAMPLER_DESC samplerDesc;

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    result = pDevice->CreateSamplerState(&samplerDesc, &mpSamplerState);

    VerifyDXResult(result);
    VerifyNotNull(mpSamplerState);

    // Describe the buffer we will use to send lighting information to the light shader.
    D3D11_BUFFER_DESC lightBufferDesc;

    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(light_buffer_t); // must always be multiple of 16.
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    // Now create the light information buffer.
    result = pDevice->CreateBuffer(&lightBufferDesc, nullptr, &mpLightBuffer);

    VerifyDXResult(result);
    VerifyNotNull(mpSamplerState);
}

void LightShader::Render(
    ID3D11DeviceContext *pDeviceContext,
    int indexCount,
    const Matrix& worldMatrix,
    const Matrix& viewMatrix,
    const Matrix& projectionMatrix,
    ID3D11ShaderResourceView *pTexture,
    const Camera& camera,
    const Light& light)
{
    Verify(IsInitialized());
    VerifyNotNull(pDeviceContext);
    VerifyNotNull(pTexture);

    SetShaderParameters(
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
    ID3D11DeviceContext *pDeviceContext,
    const Matrix& inWorldMatrix,
    const Matrix& inViewMatrix,
    const Matrix& inProjectionMatrix,
    ID3D11ShaderResourceView *pTexture,
    const Camera& camera,
    const Light& light)
{
    AssertNotNull(pDeviceContext);

    // Transpose the matrices to prepare them for the shader. This is a requirement for DirectX 11.
    Matrix worldMatrix = inWorldMatrix.Transpose();
    Matrix viewMatrix = inViewMatrix.Transpose();
    Matrix projectionMatrix = inProjectionMatrix.Transpose();

    // Lock the constant buffer so we can write to it.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = pDeviceContext->Map(mpMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    VerifyDXResult(result);
    VerifyNotNull(mappedResource.pData);

    // Get a pointer to the data in the constant buffer.
    matrix_buffer_t *pMatrixData = (matrix_buffer_t*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    pMatrixData->world = worldMatrix;
    pMatrixData->view = viewMatrix;
    pMatrixData->projection = projectionMatrix;

    // Unlock the constant buffer.
    pDeviceContext->Unmap(mpMatrixBuffer, 0);

    // Now set the updated matrix buffer in the HLSL vertex shader.
    unsigned int bufferNumber = 0;
    pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &mpMatrixBuffer);

    // Lock the camera buffer, write updated values to it and then unlock the buffer.
    result = pDeviceContext->Map(mpCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    VerifyDXResult(result);
    VerifyNotNull(mappedResource.pData);

    camera_buffer_t * pCameraData = reinterpret_cast<camera_buffer_t*>(mappedResource.pData);

    pCameraData->cameraPosition = camera.Position();
    pCameraData->padding = 0.0f;

    pDeviceContext->Unmap(mpCameraBuffer, 0);

    // Attach camera constant buffer to pixel shader.
    bufferNumber = 1;
    pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &mpCameraBuffer);

    // Set up shader texture resource in the pixel shader.
    pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

    // Lock the light constants buffer for writing.
    result = pDeviceContext->Map(mpLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    VerifyDXResult(result);
    VerifyNotNull(mappedResource.pData);

    // Copy our lighting variables into the locked constant buffer.
    light_buffer_t * pLightData = reinterpret_cast<light_buffer_t*>(mappedResource.pData);

    pLightData->ambientColor = light.AmbientColor();
    pLightData->diffuseColor = light.DiffuseColor();
    pLightData->lightDirection = light.Direction();
    pLightData->specularPower = light.SpecularPower();
    pLightData->specularColor = light.SpecularColor();

    // Unlock the constant buffer now that we are done writing values to it.
    pDeviceContext->Unmap(mpLightBuffer, 0);

    // Attach the constant buffer to the pixel shader, in the right position.
    bufferNumber = 0;
    pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &mpLightBuffer);
}

void LightShader::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount)
{
    AssertNotNull(pDeviceContext);

    // Set the vertex input layout.
    pDeviceContext->IASetInputLayout(mpLayout);

    // Set the color vertex and pixel shader.
    pDeviceContext->VSSetShader(mpVertexShader, NULL, 0);
    pDeviceContext->PSSetShader(mpPixelShader, NULL, 0);

    // Set the texture sampler state in the pixel shader.
    pDeviceContext->PSSetSamplers(0, 1, &mpSamplerState);

    // Render the object.
    pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

void LightShader::OnShutdown()
{
    ShutdownShader();
}

void LightShader::ShutdownShader()
{
    SafeRelease(mpLightBuffer);
    SafeRelease(mpCameraBuffer);
    SafeRelease(mpSamplerState);
    SafeRelease(mpMatrixBuffer);
    SafeRelease(mpLayout);
    SafeRelease(mpPixelShader);
    SafeRelease(mpVertexShader);
}