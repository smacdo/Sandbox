#include "TextureShader.h"
#include <d3d11.h>

#include "DXTest.h"

#include "BinaryBlob.h"

struct matrix_buffer_t
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
};

TextureShader::TextureShader()
: IInitializable(),
  mpVertexShader(nullptr),
  mpPixelShader(nullptr),
  mpLayout(nullptr),
  mpMatrixBuffer(nullptr),
  mpSamplerState(nullptr)
{
}

TextureShader::~TextureShader()
{
}

void TextureShader::Initialize(ID3D11Device *pDevice, HWND hwnd)
{
	VerifyNotNull(pDevice);

	if (!IsInitialized())
	{
		InitializeShader(pDevice, hwnd, ".\\Shaders\\SimpleTextureVertexShader.cso", ".\\Shaders\\SimpleTexturePixelShader.cso");
		SetInitialized();
	}
}

// break this into two functions LoadPixelShader, LoadVertexShader
// change WCHAR to std::wstring or std::string and convert last second
void TextureShader::InitializeShader(
	ID3D11Device *pDevice,
	HWND hwnd,
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

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
}

void TextureShader::Render(
	ID3D11DeviceContext *pDeviceContext,
	int indexCount,
	const DirectX::SimpleMath::Matrix& worldMatrix,
	const DirectX::SimpleMath::Matrix& viewMatrix,
	const DirectX::SimpleMath::Matrix& projectionMatrix,
	ID3D11ShaderResourceView *pTexture)
{
	Verify(IsInitialized());
	VerifyNotNull(pDeviceContext);
	VerifyNotNull(pTexture);

	SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix, pTexture);
	RenderShader(pDeviceContext, indexCount);
}

void TextureShader::SetShaderParameters(
	ID3D11DeviceContext *pDeviceContext,
	const DirectX::SimpleMath::Matrix& inWorldMatrix,
	const DirectX::SimpleMath::Matrix& inViewMatrix,
	const DirectX::SimpleMath::Matrix& inProjectionMatrix,
	ID3D11ShaderResourceView *pTexture)
{
	AssertNotNull(pDeviceContext);

	// Transpose the matrices to prepare them for the shader. This is a requirement for DirectX 11.
	DirectX::SimpleMath::Matrix worldMatrix = inWorldMatrix.Transpose();
	DirectX::SimpleMath::Matrix viewMatrix = inViewMatrix.Transpose();
	DirectX::SimpleMath::Matrix projectionMatrix = inProjectionMatrix.Transpose();

	// Lock the constant buffer so we can write to it.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = pDeviceContext->Map(mpMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VerifyDXResult(result);

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

	// Set up shader texture resource in the pixel shader.
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);
}

void TextureShader::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount)
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

void TextureShader::OnShutdown()
{
	ShutdownShader();
}

void TextureShader::ShutdownShader()
{
	SafeRelease(mpSamplerState);
	SafeRelease(mpMatrixBuffer);
	SafeRelease(mpLayout);
	SafeRelease(mpPixelShader);
	SafeRelease(mpVertexShader);
}