#include "NewInclude.hlsli"
// Constant buffer defined here
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float roughness;
    float3 cameraPosition;
    float3 ambientColor;
    Light directionalLight0;

}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    // return float4(input.uv, 0, 1);
    // return colorTint * roughness;
    // return float4(cameraPosition, 1);
    // return float4(ambientColor, 1);
    //  return float4(input.normal, 1);
    //     return float4(directionalLight0.Color, 1);

    float4 ambientTerm = float4(ambientColor, 1) * colorTint;
    input.normal = normalize(input.normal);
    
    float3 normalizedDirectionToLight = normalize(-directionalLight0.Direction);
    float3 diffuseTerm = Diffuse(input.normal, normalizedDirectionToLight) * directionalLight0.Color * colorTint.xyz;
    return float4(diffuseTerm, 1 ) + ambientTerm;

}