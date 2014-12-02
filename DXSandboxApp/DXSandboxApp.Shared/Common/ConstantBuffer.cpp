// TODO: Move .cpp, .h into Common\ folder.
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

ConstantBuffer::ConstantBuffer(ID3D11Buffer * constantBuffer)
    : mConstantBuffer()
{
    mConstantBuffer.Attach(constantBuffer);     // Transfered ownership, do not AddRef.
    //VerifyNotNull(constantBuffer);        // TODO: Implement
    //Verify(shaderSlotIndex >= 0);
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

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &rawConstantBuffer);
    DX::ThrowIfFailed(hr);

    return new ConstantBuffer(rawConstantBuffer.Detach());
}
