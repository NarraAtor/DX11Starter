#include "NewInclude.hlsli"
// Constant buffer defined here
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float roughness;
    float3 cameraPosition;
    float3 ambientColor;
    Light directionalLights[3];
    Light pointLights[1];
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
    float4 ambientTerm = float4(ambientColor, 1) * colorTint;

    // return float4(input.uv, 0, 1);
    // return colorTint * roughness;
    // return float4(cameraPosition, 1);
    // return float4(ambientColor, 1);
    //  return float4(input.normal, 1);
    //     return float4(directionalLight0.Color, 1);
    
    float4 totalDirectionalLight = float4(0, 0, 0, 0);
    
    for (int i = 0; i < 3; i++)
    {
        totalDirectionalLight += DiffuseAndSpecularForADirectionalLight(
    colorTint,
    input.normal,
    directionalLights[i].Direction,
    directionalLights[i].Color,
    cameraPosition,
    input.worldPosition,
    roughness,
    directionalLights[i].Intensity);

    }
    
    float4 totalPointLight = float4(0, 0, 0, 0);

    //for (int j = 0; j < 1; j++)
    //{
    //    totalPointLight += DiffuseSpecularAndAmbientForAPointLight(
    //colorTint,
    //input.normal,
    //pointLights[j].Position - input.worldPosition,
    //pointLights[j].Color,
    //cameraPosition,
    //input.worldPosition,
    //roughness,
    //pointLights[j].Intensity);

    //}
    return totalDirectionalLight + totalPointLight + ambientTerm;
}