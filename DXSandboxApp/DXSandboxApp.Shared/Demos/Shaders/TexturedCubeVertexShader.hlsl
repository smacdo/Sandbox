///////////////////////////////////////////////////////////////////////////////
// Vertex shader used for rendering simple models.
///////////////////////////////////////////////////////////////////////////////
cbuffer PerFrameConstantBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
}

cbuffer ModelViewProjectionConstantBuffer : register(b1)
{
    matrix model_DEPRECATED;
    matrix view;
    matrix projection;
};

cbuffer ConstantBufferChangesEveryPrim : register (b2)
{
    matrix world;
    float4 meshColor;
    float4 diffuseColor_material;
    float4 specularColor;
    float  specularExponent;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos : POSITION;
    float2 textureUV : TEXCOORD0;
    float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 textureUV : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 vertexToEye : TEXCOORD2;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output = (PixelShaderInput) 0;
    
    // Transform the vertex position into projected space.
    output.position = float4(input.pos, 1.0f);

    output.position = mul(output.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    // Pass texture coordinate to pixel shader.
    output.textureUV = input.textureUV;

    // Compute view space normal.
    output.normal = normalize(mul(mul(input.normal.xyz, (float3x3)world), (float3x3)view));

    // Vertex position in view space (normalize in pixel shader).
    output.vertexToEye = -mul(mul(input.pos, world), view).xyz;

    return output;
}
