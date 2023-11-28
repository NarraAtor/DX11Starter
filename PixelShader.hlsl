#include "NewInclude.hlsli"
// Constant buffer defined here
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float roughness;
    float3 cameraPosition;
    Light directionalLights[3];
    Light pointLights[2];
}

Texture2D Albedo : register(t0); // "t" registers for textures
Texture2D SpecularTexture : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
SamplerState BasicSampler : register(s0); // "s" registers for samplers

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
    float3 surfaceColor = Albedo.Sample(BasicSampler, input.uv).rgb;
    float specularMapValue = SpecularTexture.Sample(BasicSampler, input.uv).x;

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
    directionalLights[i].Intensity,
        specularMapValue);

    }
    
    float4 totalPointLight = float4(0, 0, 0, 0);

    for (int j = 0; j < 2; j++)
    {
        totalPointLight += DiffuseAndSpecularForAPointLight(
    colorTint,
    input.normal,
    input.worldPosition - pointLights[j].Position,
    pointLights[j].Color,
    cameraPosition,
    input.worldPosition,
    roughness,
    pointLights[j].Intensity,
    pointLights[j].Position,
    pointLights[j].Range,
        specularMapValue);

    }
    float4 finalLight = totalDirectionalLight + totalPointLight  + float4(surfaceColor, 1);
    // gamma correction
    finalLight = float4(pow(finalLight.xyz, 1.0f / 2.2f), 1);
    return finalLight;
}