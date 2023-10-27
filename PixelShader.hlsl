#include "NewInclude.hlsli"
// Constant buffer defined here
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float roughness;
    float3 cameraPosition;
    float3 ambientColor;
    //TODO: just pass all the lights here as an array
    Light directionalLight0;
    Light directionalLight1;
    Light directionalLight2;
    Light directionalLights[3];
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
    
    float4 totalDirectionalLight = float4(0, 0, 0, 0);
    
    for (int i = 0; i < 3; i++)
    {
        totalDirectionalLight += DiffuseSpecularAndAmbientForADirectionalLight(
    ambientColor,
    colorTint,
    input.normal,
    directionalLights[i].Direction,
    directionalLights[i].Color,
    cameraPosition,
    input.worldPosition,
    roughness,
    directionalLights[i].Intensity);

    }

    return totalDirectionalLight;
        return DiffuseSpecularAndAmbientForADirectionalLight(
    ambientColor,
    colorTint,
    input.normal,
    directionalLight0.Direction,
    directionalLight0.Color,
    cameraPosition,
    input.worldPosition,
    roughness,
    directionalLight0.Intensity)
    +
    DiffuseSpecularAndAmbientForADirectionalLight(
    ambientColor,
    colorTint,
    input.normal,
    directionalLight1.Direction,
    directionalLight1.Color,
    cameraPosition,
    input.worldPosition,
    roughness,
    directionalLight1.Intensity)
     +
    DiffuseSpecularAndAmbientForADirectionalLight(
    ambientColor,
    colorTint,
    input.normal,
    directionalLight2.Direction,
    directionalLight2.Color,
    cameraPosition,
    input.worldPosition,
    roughness,
    directionalLight2.Intensity);
    
}