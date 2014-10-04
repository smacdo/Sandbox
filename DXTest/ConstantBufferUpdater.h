#pragma once
#include "typedefs.h"
#include "DXSandbox.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

/**
 * \brief Assists in mapping, unmapping and updating a DirectX constant buffer.
 *
 * TODO: Specialize the class to remove unneeded switch statement.
 */
template<typename T, ShaderType shaderType, int slotIndex>
class ConstantBufferUpdater
{
public:
    ConstantBufferUpdater(ID3D11DeviceContext * pDeviceContext, ID3D11Buffer *pBufferToUpdate);
    ConstantBufferUpdater(const ConstantBufferUpdater&) = delete;
    ~ConstantBufferUpdater();

    ConstantBufferUpdater& operator = (const ConstantBufferUpdater&) = delete;
    
    HRESULT Update(
        std::function<void(T& buffer)> updateFunction) const;

private:
    ID3D11DeviceContext * mpDeviceContext;
    ID3D11Buffer * mpBufferToUpdate;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Convienence creator
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T, ShaderType shaderType, int slotIndex>
HRESULT UpdateConstantsBuffer(
    ID3D11DeviceContext * pDeviceContext,
    ID3D11Buffer * pBufferToUpdate,
    std::function<void(T& buffer)> updateFunction)
{
    ConstantBufferUpdater<T, shaderType, slotIndex> updater(pDeviceContext, pBufferToUpdate);
    return updater.Update(updateFunction);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Constant buffer updater implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T, ShaderType shaderType, int slotIndex>
ConstantBufferUpdater<T, shaderType, slotIndex>::ConstantBufferUpdater(
    ID3D11DeviceContext *pDeviceContext,
    ID3D11Buffer *pBufferToUpdate)
    : mpDeviceContext(pDeviceContext),
      mpBufferToUpdate(pBufferToUpdate)
{
    VerifyNotNull(mpDeviceContext);
    VerifyNotNull(mpBufferToUpdate);
}

template<typename T, ShaderType shaderType, int slotIndex>
ConstantBufferUpdater<T, shaderType, slotIndex>::~ConstantBufferUpdater()
{
    // Unlock buffer.
    mpDeviceContext->Unmap(mpBufferToUpdate, 0);

    // Update the constant buffer.
    ID3D11Buffer * constantBuffers[1] = { mpBufferToUpdate };

    switch (shaderType)
    {
        case ShaderType::Vertex:
            mpDeviceContext->VSSetConstantBuffers(slotIndex, 1, constantBuffers);
            break;

        case ShaderType::Pixel:
            mpDeviceContext->PSSetConstantBuffers(slotIndex, 1, constantBuffers);
            break;

        default:
            throw SandboxException(L"Unknown shader type specified when updating constant buffer");
    }
}

template<typename T, ShaderType shaderType, int slotIndex>
HRESULT ConstantBufferUpdater<T, shaderType, slotIndex>::Update(std::function<void(T& buffer)> updateFunction) const
{
    D3D11_MAPPED_SUBRESOURCE mappedChunk;
    HRESULT hr = mpDeviceContext->Map(mpBufferToUpdate, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedChunk);

    if (SUCCEEDED(hr))
    {
        VerifyNotNull(mappedChunk.pData);

        // Let user callback update the buffer.
        T * pTypedPointer = reinterpret_cast<T*>(mappedChunk.pData);
        updateFunction(*pTypedPointer);
    }

    return hr;
}