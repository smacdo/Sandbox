///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////
Texture2D shaderTexture;
SamplerState samplerType;

cbuffer Lights     // TODO: Name this something better, Lights?
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

///////////////////////////////////////////////////////////////////////////////
// Typedefs
///////////////////////////////////////////////////////////////////////////////
struct pixel_t
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

///////////////////////////////////////////////////////////////////////////////
// Vertex shader
///////////////////////////////////////////////////////////////////////////////
float4 main(pixel_t input) : SV_TARGET
{
    // Sample the pixel color from the texture using the sampler at this texture
    // coordinate location.
    float4 textureColor = shaderTexture.Sample(samplerType, input.tex);

    // Simple directional lighting calculation. The light intensity value is
    // calculate as the dot product between the triangle's normal vector and the
    // light direction vector.

    //  (XXX: Why is the direction inverted?)

    // Use the ambient color for the texel's base color.
    float4 color = ambientColor;

    // Combine the diffuse color and light intensity  to get the final amount
    // of diffuse color.
    //  TODO: We don't need this branch. Just sanity check lightIntensity when it
    //        gets passed in.
    float4 specular = 0.0;
    float lightIntensity = saturate(dot(input.normal, -lightDirection));

    if (lightIntensity > 0.0f)
    {
        // Determine final diffuse color based on current diffuse color and amount of
        // light intensity.
        color = diffuseColor * lightIntensity;
        color = saturate(color);

        // Calculate the reflection vector. [TODO: notes].
        float3 reflection = normalize(2 * lightIntensity * input.normal - lightDirection);

        // Determine the amount of specular light.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

    // Multiple texture pixel color and final diffuse color to get final pixel color.
    color = color * textureColor;
    color = saturate(color + specular);

    return color;
}