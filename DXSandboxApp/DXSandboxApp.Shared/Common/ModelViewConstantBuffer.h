#pragma once

#include <memory>

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace DirectX
{
    struct XMFLOAT4X4;
    struct XMMATRIX;
}

namespace DXSandboxApp
{
    template<class T> class TypedConstantBuffer;

    /**
     * Internal layout of the MVP constant buffer.
     */
    struct ModelViewProjectionData
    {
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
    };

    /**
     * Convience class for interacting with the MVP constant buffer present in most demos.
     */
    class ModelViewConstantBuffer
    {
    public:
        ModelViewConstantBuffer(ID3D11Device * device);
        virtual ~ModelViewConstantBuffer();

        void SetModel(const DirectX::XMMATRIX& value);
        void SetView(const DirectX::XMMATRIX& value);
        void SetProjection(const DirectX::XMMATRIX& value);

        // Update hardware constant buffer with new MVP values.
        void ApplyChanges(ID3D11DeviceContext * deviceContext);

        // Binds this buffer to an input slot in the currently active vertex shader.
        void BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

        // Binds this buffer to an input slot in the currently active pixel shader.
        void BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

    private:
        std::shared_ptr<TypedConstantBuffer<ModelViewProjectionData>> mModelViewBuffer;
    };
};

