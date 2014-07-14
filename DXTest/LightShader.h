#pragma once
#include "SimpleMath.h"
#include "IInitializable.h"

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

private:
    void InitializeShader(
        ID3D11Device* pDevice,
        const std::string& vertexShaderFile,
        const std::string& pixelShaderFile);
    virtual void OnShutdown() override;
    void ShutdownShader();
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
    ID3D11VertexShader *mpVertexShader;
    ID3D11PixelShader * mpPixelShader;
    ID3D11InputLayout * mpLayout;
    ID3D11Buffer * mpMatrixBuffer;
    ID3D11Buffer * mpCameraBuffer;
    ID3D11SamplerState * mpSamplerState;
    ID3D11Buffer * mpLightBuffer;
};

