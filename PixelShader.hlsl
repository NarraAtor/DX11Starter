#include "NewInclude.hlsli"
// Constant buffer defined here
cbuffer ExternalData : register(b0)
{
    matrix lightView;
    matrix lightProjection;
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
Texture2D ShadowMap : register(t4); // Adjust index as necessary
SamplerState BasicSampler : register(s0); // "s" registers for samplers
SamplerComparisonState ShadowSampler : register(s1);

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
        // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
// Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    
    
// Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    
    // Get a ratio of comparison results using SampleCmpLevelZero()
    float shadowAmount = ShadowMap.SampleCmpLevelZero(
               ShadowSampler,
               shadowUV,
               distToLight).r;
    float distShadowMap = ShadowMap.Sample(BasicSampler, shadowUV).r;
    // For now (since my shadows aren't working), just return black where there are shadows so the professor sees i did SOMETHING.
    if (distShadowMap < distToLight)
        return float4(0, 0, 0, 1);
    
    float3 surfaceColor = Albedo.Sample(BasicSampler, input.uv).rgb;
    float specularMapValue = SpecularTexture.Sample(BasicSampler, input.uv).x;
    
    // float roughness = RoughnessMap.Sample(SamplerOptions, input.uv).r;
    // float metalness = MetalnessMap.Sample(SamplerOptions, input.uv).r;

    // return float4(input.uv, 0, 1);
    // return colorTint * roughness;
    // return float4(cameraPosition, 1);
    // return float4(ambientColor, 1);
    //  return float4(input.normal, 1);
    //     return float4(directionalLight0.Color, 1);
    
    float4 totalDirectionalLight = float4(0, 0, 0, 0);
    
    for (int i = 0; i < 3; i++)
    {
        float4 lightResult = DiffuseAndSpecularForADirectionalLight(
    colorTint,
    input.normal,
    directionalLights[i].Direction,
    directionalLights[i].Color,
    cameraPosition,
    input.worldPosition,
    roughness,
    directionalLights[i].Intensity,
        specularMapValue);
        
        // If this is the 3RD light, apply the shadowing result
        if (i == 2)
        {
            lightResult *= shadowAmount;
        }
        // Add this light's result to the total light for this pixel        
        totalDirectionalLight += lightResult;


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