#include "ColorShader.h"
#include "DXTest.h"

#include "BinaryBlob.h"

struct matrix_buffer_t
{
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
};

ColorShader::ColorShader()
: mInitialized(false),
  mpVertexShader(nullptr),
  mpPixelShader(nullptr),
  mpLayout(nullptr),
  mpMatrixBuffer(nullptr)
{
}

ColorShader::~ColorShader()
{
	Shutdown();
}

void ColorShader::Initialize(ID3D11Device *pDevice, HWND hwnd)
{
	VerifyNotNull(pDevice);

	if (mInitialized)
	{
		return;
	}

	InitializeShader(pDevice, hwnd, "../DXTest/Shaders/ColorVS.fxo", "../DXTest/Shaders/ColorPS.fxo");
	mInitialized = true;
}

// break this into two functions LoadPixelShader, LoadVertexShader
// change WCHAR to std::wstring or std::string and convert last second
void ColorShader::InitializeShader(
	ID3D11Device *pDevice,
	HWND hwnd,
	char * pVertexShaderFile,
	char * pPixelShaderFile)
{
	// Load the pixel and vertex shaders.
	BinaryBlob vertexShaderBlob = BinaryBlob::LoadFromFile(pVertexShaderFile);
	BinaryBlob pixelShaderBlob = BinaryBlob::LoadFromFile(pPixelShaderFile);

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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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
}

void ColorShader::Render(
	ID3D11DeviceContext *pDeviceContext,
	int indexCount,
	const DirectX::SimpleMath::Matrix& worldMatrix,
	const DirectX::SimpleMath::Matrix& viewMatrix,
	const DirectX::SimpleMath::Matrix& projectionMatrix)
{
	VerifyNotNull(pDeviceContext);

	if (!mInitialized)
	{
		return;
	}

	SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix);
	RenderShader(pDeviceContext, indexCount);
}

void ColorShader::SetShaderParameters(
	ID3D11DeviceContext *pDeviceContext,
	const DirectX::SimpleMath::Matrix& inWorldMatrix,
	const DirectX::SimpleMath::Matrix& inViewMatrix,
	const DirectX::SimpleMath::Matrix& inProjectionMatrix)
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
}

void ColorShader::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount)
{
	AssertNotNull(pDeviceContext);

	// Set the vertex input layout.
	pDeviceContext->IASetInputLayout(mpLayout);

	// Set the color vertex and pixel shader.
	pDeviceContext->VSSetShader(mpVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(mpPixelShader, NULL, 0);

	// Render the object.
	pDeviceContext->DrawIndexed(indexCount, 0, 0);
}

void ColorShader::Shutdown()
{
	if (!mInitialized)
	{
		return;
	}

	ShutdownShader();
	mInitialized = false;
}

void ColorShader::ShutdownShader()
{
	SafeRelease(mpMatrixBuffer);
	SafeRelease(mpLayout);
	SafeRelease(mpPixelShader);
	SafeRelease(mpVertexShader);
}