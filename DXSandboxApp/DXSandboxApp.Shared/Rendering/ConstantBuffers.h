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

    /**
     * Lighting constant buffer.
     */
    struct SceneLightingData
    {
        DirectX::XMFLOAT4 position[4];
        DirectX::XMFLOAT4 color;
    };

    /**
    * Convience class for interacting with the MVP constant buffer present in most demos.
    */
    class SceneLightingConstantBuffer       // TODO: Rename PerSceneConstantBuffer
    {
    public:
        SceneLightingConstantBuffer(ID3D11Device * device);
        virtual ~SceneLightingConstantBuffer();

        void SetPosition(DirectX::XMFLOAT4 position, unsigned int index);
        void SetColor(DirectX::XMFLOAT4 color);

        // Update hardware constant buffer with new MVP values.
        void ApplyChanges(ID3D11DeviceContext * deviceContext);

        // Binds this buffer to an input slot in the currently active vertex shader.
        void BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

        // Binds this buffer to an input slot in the currently active pixel shader.
        void BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

    private:
        std::shared_ptr<TypedConstantBuffer<SceneLightingData>> mData;
    };

    /**
     * Per primitive data.
     */
    struct PerPrimitiveData
    {
        DirectX::XMFLOAT4X4 worldMatrix;
        DirectX::XMFLOAT4 meshColor;
        DirectX::XMFLOAT4 diffuseColor;
        DirectX::XMFLOAT4 specularColor;
        float specularExponent;
        DirectX::XMFLOAT3 extra;
    };

    /**
    * Convience class for interacting with the MVP constant buffer present in most demos.
    */
    class PerPrimitiveConstantBuffer
    {
    public:
        PerPrimitiveConstantBuffer(ID3D11Device * device);
        virtual ~PerPrimitiveConstantBuffer();

        void SetWorldMatrix(const DirectX::XMMATRIX& value);
        void SetMeshColor(DirectX::XMFLOAT4 meshColor);
        void SetDiffuseColor(DirectX::XMFLOAT4 diffuseColor);
        void SetSpecularColor(DirectX::XMFLOAT4 specularColor);
        void SetSpecularExponent(float specularExponent);

        // Update hardware constant buffer with new MVP values.
        void ApplyChanges(ID3D11DeviceContext * deviceContext);

        // Binds this buffer to an input slot in the currently active vertex shader.
        void BindToActiveVertexShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

        // Binds this buffer to an input slot in the currently active pixel shader.
        void BindToActivePixelShader(ID3D11DeviceContext * deviceContext, unsigned int startSlot) const;

    private:
        std::shared_ptr<TypedConstantBuffer<PerPrimitiveData>> mData;
    };
};

