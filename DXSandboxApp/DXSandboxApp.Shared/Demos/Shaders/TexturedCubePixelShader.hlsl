///////////////////////////////////////////////////////////////////////////////
// Pixel shader used for rendering simple models.
///////////////////////////////////////////////////////////////////////////////
Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

cbuffer PerFrameConstantBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
}

cbuffer ModelViewProjectionConstantBuffer : register(b1)
{
    matrix model;           // TODO: Unused, only used for earlier demos.
    matrix view;
    matrix projection;
};

cbuffer ConstantBufferChangesEveryPrim : register (b2)
{
    matrix world;
    float4 meshColor;
    float4 diffuseColor_material;
    float4 specularColor;
    float specularExponent;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 textureUV : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 vertexToEye : TEXCOORD2;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.textureUV);

    // Use the ambient color for the texel's base color.
    float4 color = diffuseColor;

    // Multiple texture pixel color and final diffuse color to get final pixel color.
    color = color * textureColor;
    color = saturate(color);

    return color;
}
