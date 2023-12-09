#include "NewInclude.hlsli"
// Constant buffer defined here
cbuffer ExternalData : register(b0)
{
    // matrix lightView;
    // matrix lightProjection;
    float4 colorTint;
    float roughness;
    float3 cameraPosition;
    Light directionalLights[3];
    Light pointLights[2];
}

Texture2D Albedo : register(t0); // "t" registers for textures
//Texture2D SpecularTexture : register(t1);
Texture2D RoughnessMap : register(t1);
Texture2D MetalnessMap : register(t2);
Texture2D NormalMap : register(t3);
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
float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
          // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
// Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
// Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    float distShadowMap = ShadowMap.Sample(BasicSampler, shadowUV).r;
// For testing, just return black where there are shadows.
    if (distShadowMap < distToLight)
        return float4(0, 0, 0, 1);
    
    float3 surfaceColor = Albedo.Sample(BasicSampler, input.uv).rgb;
    // "un-correction"
    surfaceColor = pow(surfaceColor, 2.2f);
    
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    //float roughness = 0.1f;
    //float metalness = 1.0f;
    
    //float specularMapValue = SpecularTexture.Sample(BasicSampler, input.uv).x;
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);
    
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal); // Don’t forget to normalize!

    // Feel free to adjust/simplify this code to fit with your existing shader(s)
    // Simplifications include not re-normalizing the same vector more than once!
    float3 N = normalize(input.normal); // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    input.normal = mul(unpackedNormal, TBN);
    
    float4 totalDirectionalLight = float4(0, 0, 0, 0);
    
    for (int i = 0; i < 3; i++)
    {
    //    totalDirectionalLight += DiffuseAndSpecularForADirectionalLight(
    //colorTint,
    //input.normal,
    //directionalLights[i].Direction,
    //directionalLights[i].Color,
    //cameraPosition,
    //input.worldPosition,
    //roughness,
    //directionalLights[i].Intensity,
    //    specularMapValue);
        float3 directionToLight = normalize(-directionalLights[i].Direction);
        float3 directionToCamera = normalize(cameraPosition - input.worldPosition);
        float diff = DiffusePBR(input.normal, directionToLight);
        float3 F;
        float3 spec = 
        MicrofacetBRDF(
        input.normal, 
        directionToLight, 
        directionToCamera, 
        roughness, 
        specularColor, 
        F);
        
        // Calculate diffuse with energy conservation, including cutting diffuse for metals
        float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
        
        // Combine the final diffuse and specular values for this light
        totalDirectionalLight += float4((balancedDiff * surfaceColor + spec) * directionalLights[i].Intensity * directionalLights[i].Color, 1);

    }
    
    float4 totalPointLight = float4(0, 0, 0, 0);

    for (int j = 0; j < 2; j++)
    {
    //    totalPointLight += DiffuseAndSpecularForAPointLight(
    //colorTint,
    //input.normal,
    //input.worldPosition - pointLights[j].Position,
    //pointLights[j].Color,
    //cameraPosition,
    //input.worldPosition,
    //roughness,
    //pointLights[j].Intensity,
    //pointLights[j].Position,
    //pointLights[j].Range,
    //    specularMapValue);

        float3 directionToLight = normalize(-pointLights[j].Direction);
        float3 directionToCamera = normalize(cameraPosition - input.worldPosition);
        float diff = DiffusePBR(input.normal, directionToLight);
        float3 F;
        float3 spec =
        MicrofacetBRDF(
        input.normal,
        directionToLight,
        directionToCamera,
        roughness,
        specularColor,
        F);
        
         // Calculate diffuse with energy conservation, including cutting diffuse for metals
        float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
        
        
        // Combine the final diffuse and specular values for this light
        float4 preAttenuation = float4((balancedDiff * surfaceColor + spec) * pointLights[j].Intensity * pointLights[j].Color, 1);
        totalPointLight += preAttenuation * Attenuate(pointLights[j].Position, pointLights[j].Range, input.worldPosition);

    }
    
    float4 finalLight = totalDirectionalLight + totalPointLight;
    // gamma correction
    finalLight = float4(pow(finalLight.xyz, 1.0f / 2.2f), 1);
    return finalLight;
    // return float4(input.tangent, 1);
}