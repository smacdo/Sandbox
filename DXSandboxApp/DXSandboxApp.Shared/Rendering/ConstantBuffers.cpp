#include "pch.h"
#include "ConstantBuffers.h"
#include "Common\ConstantBuffer.h"

using namespace DXSandboxApp;
using namespace DirectX;

ModelViewConstantBuffer::ModelViewConstantBuffer(ID3D11Device * device)
    : mModelViewBuffer(new TypedConstantBuffer<ModelViewProjectionData>(device))
{
}

ModelViewConstantBuffer::~ModelViewConstantBuffer()
{
}

void ModelViewConstantBuffer::SetModel(const XMMATRIX& value)
{
    XMStoreFloat4x4(&mModelViewBuffer->Data().model, XMMatrixTranspose(value));
}

void ModelViewConstantBuffer::SetView(const XMMATRIX& value)
{
    XMStoreFloat4x4(&mModelViewBuffer->Data().view, XMMatrixTranspose(value));
}

void ModelViewConstantBuffer::SetProjection(const XMMATRIX& value)
{
    XMStoreFloat4x4(&mModelViewBuffer->Data().projection, XMMatrixTranspose(value));
}

void ModelViewConstantBuffer::ApplyChanges(ID3D11DeviceContext * deviceContext)
{
    mModelViewBuffer->ApplyChanges(deviceContext);
}

void ModelViewConstantBuffer::BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mModelViewBuffer->BindToActiveVertexShader(deviceContext, startSlot);
}

void ModelViewConstantBuffer::BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mModelViewBuffer->BindToActivePixelShader(deviceContext, startSlot);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

SceneLightingConstantBuffer::SceneLightingConstantBuffer(ID3D11Device * device)
    : mData(new TypedConstantBuffer<SceneLightingData>(device))
{
}

SceneLightingConstantBuffer::~SceneLightingConstantBuffer()
{
}

void SceneLightingConstantBuffer::SetPosition(DirectX::XMFLOAT4 position, unsigned int index)
{
    mData->Data().position[index] = position;
}

void SceneLightingConstantBuffer::SetColor(DirectX::XMFLOAT4 color)
{
    mData->Data().color = color;
}

void SceneLightingConstantBuffer::ApplyChanges(ID3D11DeviceContext * deviceContext)
{
    mData->ApplyChanges(deviceContext);
}

void SceneLightingConstantBuffer::BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mData->BindToActiveVertexShader(deviceContext, startSlot);
}

void SceneLightingConstantBuffer::BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mData->BindToActivePixelShader(deviceContext, startSlot);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

PerPrimitiveConstantBuffer::PerPrimitiveConstantBuffer(ID3D11Device * device)
    : mData(new TypedConstantBuffer<PerPrimitiveData>(device))
{
}

PerPrimitiveConstantBuffer::~PerPrimitiveConstantBuffer()
{
}

void PerPrimitiveConstantBuffer::SetWorldMatrix(const DirectX::XMMATRIX& value)
{
    XMStoreFloat4x4(&mData->Data().worldMatrix, XMMatrixTranspose(value));
}

void PerPrimitiveConstantBuffer::SetMeshColor(DirectX::XMFLOAT4 meshColor)
{
    mData->Data().meshColor = meshColor;
}

void PerPrimitiveConstantBuffer::SetDiffuseColor(DirectX::XMFLOAT4 diffuseColor)
{
    mData->Data().diffuseColor = diffuseColor;
}

void PerPrimitiveConstantBuffer::SetSpecularColor(DirectX::XMFLOAT4 specularColor)
{
    mData->Data().specularColor = specularColor;
}

void PerPrimitiveConstantBuffer::SetSpecularExponent(float specularExponent)
{
    mData->Data().specularExponent = specularExponent;
}

void PerPrimitiveConstantBuffer::ApplyChanges(ID3D11DeviceContext * deviceContext)
{
    mData->ApplyChanges(deviceContext);
}

void PerPrimitiveConstantBuffer::BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mData->BindToActiveVertexShader(deviceContext, startSlot);
}

void PerPrimitiveConstantBuffer::BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mData->BindToActivePixelShader(deviceContext, startSlot);
}