///////////////////////////////////////////////////////////////////////////////
// Vertex shader used for rendering simple models.
///////////////////////////////////////////////////////////////////////////////
// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos : POSITION;
    float2 textureUV : TEXCOORD0;
//    float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput     // TODO: Shared.
{
    float4 pos : SV_POSITION;
    float2 textureUV : TEXCOORD0;
//    float3 normal : NORMAL;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output = (PixelShaderInput) 0;
    
    // Transform the vertex position into projected space.
    float4 pos = float4(input.pos, 1.0f);

    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.pos = pos;

    // Pass texture coordinate to pixel shader.
    output.textureUV = input.textureUV;

    // Transform normal vector against world matrix. (Since normal is in model space).
//    output.normal = mul(input.normal, (float3x3) view);
//    output.normal = normalize(output.normal);

    return output;
}
