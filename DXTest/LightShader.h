#pragma once
#include "SimpleMath.h"
#include "IInitializable.h"

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
class Light;
class Camera;

class LightShader : public IInitializable
{
public:
    LightShader();
    LightShader(const LightShader&) = delete;
    virtual ~LightShader() override;

    LightShader& operator =(const LightShader&) = delete;

    void Initialize(ID3D11Device *pDevice);
    void Render(
        ID3D11DeviceContext *pContext,
        int indexCount,
        const DirectX::SimpleMath::Matrix&,
        const DirectX::SimpleMath::Matrix&,
        const DirectX::SimpleMath::Matrix&,
        ID3D11ShaderResourceView * pTexture,
        const Camera& camera,
        const Light& light);

protected:
    virtual void OnShutdown() override;

private:
    void InitializeShader(
        ID3D11Device* pDevice,
        const std::wstring& vertexShaderFile,
        const std::wstring& pixelShaderFile);
    
    void SetShaderParameters(
        ID3D11DeviceContext *pContext,
        const DirectX::SimpleMath::Matrix&,
        const DirectX::SimpleMath::Matrix&,
        const DirectX::SimpleMath::Matrix&,
        ID3D11ShaderResourceView * pTexture,
        const Camera& camera,
        const Light& light);

    void RenderShader(ID3D11DeviceContext*, int);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mMatrixBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mCameraBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mLightBuffer;
};

