#include "pch.h"
#include "ConstantBuffer.h"
#include "Common/DirectXHelper.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Platform;

using namespace DXSandboxApp;

ConstantBuffer::ConstantBuffer(ID3D11Device * pDevice, size_t bufferSize)
    : mConstantBuffer()
{
    CD3D11_BUFFER_DESC bufferDesc(bufferSize, D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(pDevice->CreateBuffer(&bufferDesc, nullptr, &mConstantBuffer));
}

ConstantBuffer::ConstantBuffer(ID3D11Buffer * pConstantBuffer)
    : mConstantBuffer()
{
    CheckNotNull(pConstantBuffer);
    mConstantBuffer.Attach(pConstantBuffer);     // Transfered ownership, do not AddRef.
}

ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    ID3D11Buffer * constantBuffers[1] = { mConstantBuffer.Get() };
    deviceContext->VSSetConstantBuffers(startSlot, 1, constantBuffers);
}

void ConstantBuffer::BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const
{
    ID3D11Buffer * constantBuffers[1] = { mConstantBuffer.Get() };
    deviceContext->PSSetConstantBuffers(startSlot, 1, constantBuffers);
}

void ConstantBuffer::UpdateValue(ID3D11DeviceContext * deviceContext, const void * pData)
{
    deviceContext->UpdateSubresource(
        mConstantBuffer.Get(),      // Constant buffer resource object.
        0,                          // Destination resource sub-index.
        nullptr,                    // Destination box layout
        pData,                      // Pointer to source data (must be same size as dest buffer)
        0,                          // Size of one row of source data.
        0);                         // Size of one depth slice of source data.
}

ConstantBuffer * ConstantBuffer::Create(ID3D11Device * device, size_t bufferSize)
{
    CD3D11_BUFFER_DESC bufferDesc(bufferSize, D3D11_BIND_CONSTANT_BUFFER);
    ComPtr<ID3D11Buffer> rawConstantBuffer;

    HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &rawConstantBuffer);
    DX::ThrowIfFailed(hr);

    return new ConstantBuffer(rawConstantBuffer.Detach());
}