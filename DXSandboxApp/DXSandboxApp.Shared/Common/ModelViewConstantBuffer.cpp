#include "pch.h"
#include "ModelViewConstantBuffer.h"
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

void ModelViewConstantBuffer::BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mModelViewBuffer->BindToActiveVertexShader(deviceContext, startSlot);
}

void ModelViewConstantBuffer::BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    mModelViewBuffer->BindToActivePixelShader(deviceContext, startSlot);
}

void ModelViewConstantBuffer::ApplyChanges(ID3D11DeviceContext * deviceContext)
{
    mModelViewBuffer->ApplyChanges(deviceContext);
}