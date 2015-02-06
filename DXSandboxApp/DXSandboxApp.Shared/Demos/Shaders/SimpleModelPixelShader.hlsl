///////////////////////////////////////////////////////////////////////////////
// Pixel shader used for rendering simple models.
///////////////////////////////////////////////////////////////////////////////
// Per-pixel color data passed through the pixel shader.
Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

struct PixelShaderInput     // TODO: Shared.
{
    float4 pos : SV_POSITION;
    float2 textureUV : TEXCOORD0;
//    float3 normal : NORMAL;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    return diffuseTexture.Sample(linearSampler, input.textureUV);
}
