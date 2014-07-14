// Simple light shader.

///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

///////////////////////////////////////////////////////////////////////////////
// Typedefs
///////////////////////////////////////////////////////////////////////////////
struct VIn
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VOut
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex shader.
///////////////////////////////////////////////////////////////////////////////
VOut main(VIn input)
{
    VOut output;

    // Widen position vector to 4 units.
    input.position.w = 1.0f;

    // Calculate vertex position.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Save the texture coordinate for pixel shader.
    output.tex = input.tex;

    // Transform normal vector (from model space?) to world space, and then renormalize
    // the matrix.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // Calculate the viewing angle, which is a vector from this vertex to the camera's
    // position. This is done by obtaining the vertex's world position and subtracting
    // it from the camera position.
    float4 worldPosition = mul(input.position, worldMatrix);
    output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);

    return output;
}