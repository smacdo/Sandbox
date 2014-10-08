#include "FontShader.h"
#include "Font.h"
#include "Texture.h"
#include "DXSandbox.h"
#include "BinaryBlob.h"
#include "SimpleMath.h"
#include "DXTestException.h"
#include "Dx3d.h"

#include "ConstantBufferUpdater.h"

#include <d3d11.h>

using namespace DirectX::SimpleMath;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Font shader constants
///////////////////////////////////////////////////////////////////////////////////////////////////
static const wchar_t FontVertexShaderFilePath[] = L".\\Shaders\\FontVertexShader.cso";
static const wchar_t FontPixelShaderFilePath[] = L".\\Shaders\\FontPixelShader.cso";

struct matrix_buffer_t
{
	Matrix world;
	Matrix view;
	Matrix projection;
};

struct pixel_buffer_t       // TODO: terrible name
{
    Vector4 pixelColor;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Font shader implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
FontShader::FontShader()
: IInitializable(),
  mVertexShader(),
  mPixelShader(),
  mLayout(),
  mMatrixBuffer(),
  mSamplerState(),
  mPixelBuffer()
{
}

FontShader::~FontShader()
{
}

void FontShader::Initialize(Dx3d& dx)
{
	if (!IsInitialized())
	{
		HRESULT hr = InitializeShader(dx);
        VerifyDXResult(hr);

		SetInitialized();
	}
}

HRESULT FontShader::InitializeShader(Dx3d& dx)
{
    HRESULT hr = S_OK;

    // Load and create the pixel and vertex shaders.
    {
        BinaryBlob vertexShaderBlob = BinaryBlob::LoadFromFile(FontVertexShaderFilePath);
        BinaryBlob pixelShaderBlob = BinaryBlob::LoadFromFile(FontPixelShaderFilePath);

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

    // Create constant buffers for MVP camera projection and pixel shader info.
    if (SUCCEEDED(hr))
    {
        hr = dx.CreateConstantBuffer(sizeof(matrix_buffer_t), &mMatrixBuffer);
    }

    if (SUCCEEDED(hr))
    {
        hr = dx.CreateConstantBuffer(sizeof(pixel_buffer_t), &mPixelBuffer);
    }

    // Create a texture sampler state description.
    if (SUCCEEDED(hr))
    {
        hr = dx.CreateTextureSamplerState(&mSamplerState);
    }

    return hr;
}

HRESULT FontShader::CreateInputLayout(
    Dx3d& dx,
    const BinaryBlob& vertexShaderBlob,
    ID3D11InputLayout **ppLayoutOut) const
{
    VerifyNotNull(ppLayoutOut);
    *ppLayoutOut = nullptr;

    // Describe the layout of data that will be fed to this shader.
    // This layout needs to match the vertex type structure defined in the model class and shader.
    const size_t INPUT_ELEMENT_COUNT = 2;
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

void FontShader::Render(
	Dx3d& dx,
    const Font& font,
	int indexCount,
	const Matrix& worldMatrix,
	const Matrix& viewMatrix,
	const Matrix& projectionMatrix,
    const Vector4& pixelColor)
{
    if (!IsInitialized()) { throw NotInitializedException(L"FontShader"); }

    SetShaderParameters(dx, font, worldMatrix, viewMatrix, projectionMatrix, pixelColor);
	RenderShader(dx, indexCount);
}

void FontShader::SetShaderParameters(
	Dx3d& dx,
    const Font& font,
	const Matrix& inWorldMatrix,
	const Matrix& inViewMatrix,
	const Matrix& inProjectionMatrix,
    const Vector4& pixelColor)
{
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

    // Update the pixel shader constant buffer to have the latest pixel color.
    hr = UpdateConstantsBuffer<pixel_buffer_t, ShaderType::Pixel, 0>(
        dx.GetDeviceContext(),
        mPixelBuffer.Get(),
        [&](pixel_buffer_t& p)
        {
            p.pixelColor = pixelColor;
    });

    // Set up shader texture resource in the pixel shader.
    const ID3D11ShaderResourceView* resourceViews[1] = { font.GetTexture().GetShaderResourceView() };
    dx.GetDeviceContext()->PSSetShaderResources(
        0,
        1,
        const_cast<ID3D11ShaderResourceView* const *>(resourceViews));  // TODO: Is there a way to avoid const cast?
}

void FontShader::RenderShader(Dx3d& dx, int indexCount)
{
	// Set the vertex input layout.
    dx.GetDeviceContext()->IASetInputLayout(mLayout.Get());

	// Set the color vertex and pixel shader.
    dx.GetDeviceContext()->VSSetShader(mVertexShader.Get(), NULL, 0);
    dx.GetDeviceContext()->PSSetShader(mPixelShader.Get(), NULL, 0);

	// Set the texture sampler state in the pixel shader.
    ID3D11SamplerState* samplerStates[1] = { mSamplerState.Get() };
    dx.GetDeviceContext()->PSSetSamplers(0, 1, samplerStates);

	// Render the object.
    dx.GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

void FontShader::OnShutdown()
{
}