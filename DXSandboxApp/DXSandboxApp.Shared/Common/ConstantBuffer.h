#pragma once

#include <wrl.h>
#include <functional>

#include "Common\DirectXHelper.h"

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace DXSandboxApp
{
    /**
     * \brief Assists in manipulating and updating a DirectX constant buffer.
     * TODO: Consider making Update() a virtual function and returning the correct update class object.
     */
    class ConstantBuffer
    {
    public:
        // Default constant buffer constructor.
        ConstantBuffer(ID3D11Device * device, size_t bufferSize);

        // Disabled copy constructor.
        ConstantBuffer(const ConstantBuffer&) = delete;

        // Destructor.
        virtual ~ConstantBuffer();

        // Disabled assignment operator.
        ConstantBuffer& operator = (const ConstantBuffer&) = delete;

        // Update constant buffer.
        //  TODO: Use static assertion / template trait to force T to be a POD.
        template<class T>
        HRESULT Update(ID3D11DeviceContext * deviceContext, std::function<void(T& buffer)> updateFunction);

        // Update constant buffer with the provided data.
        //  TODO: Use static assertion / template trait to force T to be a POD
        template<class T>
        void UpdateValue(ID3D11DeviceContext * deviceContext, const T& constantData);

        void UpdateValue(ID3D11DeviceContext * deviceContext, const void * pData);

        // Binds this constant shader to the currently active vertex shader.
        void BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

        // Binds this constant shader to the currently active pixel shader.
        void BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

        // Create a new typed constant buffer.
        template<typename T>
        static ConstantBuffer * Create(ID3D11Device * device);

        // Create a new untyped constant buffer.
        static ConstantBuffer * Create(ID3D11Device * device, size_t bufferSize);

    protected:
        // Internal constructor.
        ConstantBuffer(
            ID3D11Buffer * constantBuffer);     // Transfer ownership of object (do not release)

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Implementation of constant buffer methods.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T>
    HRESULT ConstantBuffer::Update(ID3D11DeviceContext * deviceContext, std::function<void(T& buffer)> updateFunction)
    {
        // TODO: Verify deviceContext != nullptr.


        // Lock and map the buffer to a typed pointer.
        D3D11_MAPPED_SUBRESOURCE mappedChunk;
        HRESULT hr = pDeviceContext->Map(mConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedChunk);

        if (SUCCEEDED(hr))
        {
            updateFunction(reinterpret_cast<T*>(mappedChunk.pData));
            pDeviceContext->Unmap(mConstantBuffer.Get(), 0);
        }

        return hr;
    }

    template<class T>
    void ConstantBuffer::UpdateValue(ID3D11DeviceContext * deviceContext, const T& data)
    {
        // TODO: Verify deviceContext != nullptr.
        UpdateValue(deviceContext, reinterpret_cast<const void *>(&data));
    }

    template<class T>
    ConstantBuffer * ConstantBuffer::Create(ID3D11Device * pDevice)
    {
        return Create(pDevice, sizeof(T));
    }

    /**
     * \brief Assists in manipulating and updating a DirectX typed constant buffer with backing storage.
     */
    template<class T>
    class TypedConstantBuffer
    {
    public:
        TypedConstantBuffer(ID3D11Device * device);

        // Disabled copy constructor.
        TypedConstantBuffer(const ConstantBuffer&) = delete;

        // Destructor.
        virtual ~TypedConstantBuffer();

        // Disabled assignment operator.
        TypedConstantBuffer& operator = (const TypedConstantBuffer&) = delete;

        // Get backing data storage.
        T& Data();

        // Get backing data storage via constant reference.
        const T& Data() const;

        // Apply backing data changes to the hardware constant buffer.
        void ApplyChanges(ID3D11DeviceContext * deviceContext);

        // Set constant buffer value, and immediately update the buffer.
        void SetValue(ID3D11DeviceContext * deviceContext, const T& value);

        // Binds this buffer to an input slot in the currently active vertex shader.
        void BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

        // Binds this buffer to an input slot in the currently active pixel shader.
        void BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

        // Create a new typed constant buffer.
        static TypedConstantBuffer<T> * Create(ID3D11Device * device);

    protected:
        // Internal constructor.
        TypedConstantBuffer(
            ID3D11Buffer * constantBuffer);     // Transfer ownership of object (do not release)

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
        typename T mData;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Typed constant buffer implementation.
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T>
    TypedConstantBuffer<T>::TypedConstantBuffer(ID3D11Device * device)
        : mConstantBuffer(),
          mData()
    {
        CD3D11_BUFFER_DESC bufferDesc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
        HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &mConstantBuffer);
        DX::ThrowIfFailed(hr);
    }

    template<class T>
    TypedConstantBuffer<T>::TypedConstantBuffer(ID3D11Buffer * constantBuffer)
        : mConstantBuffer(),
          mData()
    {
        mConstantBuffer.Attach(constantBuffer);
    }

    template<class T>
    TypedConstantBuffer<T>::~TypedConstantBuffer()
    {
    }

    template<class T>
    T& TypedConstantBuffer<T>::Data()
    {
        return mData;
    }

    template<class T>
    const T& TypedConstantBuffer<T>::Data() const
    {
        return mData;
    }

    template<class T>
    void TypedConstantBuffer<T>::ApplyChanges(ID3D11DeviceContext * deviceContext)
    {
        // TODO: Verify deviceContext != nullptr.
        deviceContext->UpdateSubresource(
            mConstantBuffer.Get(),      // Constant buffer resource object.
            0,                          // Destination resource sub-index.
            nullptr,                    // Destination box layout
            &mData,                     // Pointer to source data (must be same size as dest buffer)
            0,                          // Size of one row of source data.
            0);                         // Size of one depth slice of source data.
    }

    template<class T>
    void TypedConstantBuffer<T>::SetValue(ID3D11DeviceContext * deviceContext, const T& value)
    {
        mData = value;
        ApplyChanges();
    }

    template<class T>
    void TypedConstantBuffer<T>::BindToActiveVertexShader(
        ID3D11DeviceContext * deviceContext,
        unsigned int startSlot) const
    {
        ID3D11Buffer * constantBuffers[1] = { mConstantBuffer.Get() };
        deviceContext->VSSetConstantBuffers(startSlot, 1, constantBuffers);
    }

    template<class T>
    void TypedConstantBuffer<T>::BindToActivePixelShader(
        ID3D11DeviceContext * deviceContext,
        unsigned int startSlot) const
    {
        ID3D11Buffer * constantBuffers[1] = { mConstantBuffer.Get() };
        deviceContext->PSSetConstantBuffers(startSlot, 1, constantBuffers);
    }

    template<class T>
    TypedConstantBuffer<T> * TypedConstantBuffer<T>::Create(ID3D11Device * pDevice)
    {
        CD3D11_BUFFER_DESC bufferDesc(bufferSize, D3D11_BIND_CONSTANT_BUFFER);
        ComPtr<ID3D11Buffer> rawConstantBuffer;

        HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &rawConstantBuffer);
        DX::ThrowIfFailed(hr);

        return new TypedConstantBuffer<T>(rawConstantBuffer.Detach());
    }
}
