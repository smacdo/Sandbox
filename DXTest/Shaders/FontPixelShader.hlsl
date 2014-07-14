// Simple texture shader.

///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////
Texture2D shaderTexture;
SamplerState sampleType;

cbuffer PixelBuffer
{
    float4 pixelColor;
};

///////////////////////////////////////////////////////////////////////////////
// Typedefs
///////////////////////////////////////////////////////////////////////////////
struct pixel_t
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex shader
///////////////////////////////////////////////////////////////////////////////
float4 main(pixel_t input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture
	// coordinate location.
    float4 color = shaderTexture.Sample(sampleType, input.tex);
    //float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);

    // Treat black as a transparent color. Otherwise use the assigned pixel color
    // as the fonts color.
    if (color.r == 0.0f)
    {
        color.a = 0.0f;
    }
    else
    {
        color.a = 1.0f;
        color = saturate(color * pixelColor);
    }

	return color;
}