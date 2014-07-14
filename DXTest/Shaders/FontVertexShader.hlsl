// Simple texture shader.

///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

///////////////////////////////////////////////////////////////////////////////
// Typedefs
///////////////////////////////////////////////////////////////////////////////
struct vertex_t
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct pixel_t
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex shader
///////////////////////////////////////////////////////////////////////////////
pixel_t main(vertex_t input)
{
    pixel_t output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view and projection
	// matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shade.r
	output.tex = input.tex;

	return output;
}