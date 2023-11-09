#ifndef NEW_INCLUDE // Each .hlsli file needs a unique identifier!
#define NEW_INCLUDE

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f


struct Light
{
    int Type; // Which kind of light? 0, 1 or 2 (see above)
    float3 Direction; // Directional and Spot lights need a direction
    float Range; // Point and Spot lights have a max range for attenuation
    float3 Position; // Point and Spot lights have a position in space
    float Intensity; // All lights need an intensity
    float3 Color; // All lights need a color
    float SpotFalloff; // Spot lights need a value to define their “cone” size
    float3 Padding; // Purposefully padding to hit the 16-byte boundary
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float2 uv : TEXCOORD; // UV Maps
    float3 worldPosition : POSITION; // XYZ Position
    float3 normal : NORMAL; // Normal vectors
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel_NormalMap
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float2 uv : TEXCOORD; // UV Maps
    float3 worldPosition : POSITION; // XYZ Position
    float3 normal : NORMAL; // Normal vectors
    float3 tangent : TANGENT; // Tangent of texture
};


float3 Diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

float PhongSpecular(float3 reflectionVector, float3 surfaceToCameraVector, float roughness)
{
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    if(specExponent <= 0.05f)
    {
        return 0;
    }
    return pow(saturate(dot(reflectionVector, surfaceToCameraVector)), specExponent);
}

float Attenuate(float3 lightPosition, float lightRange, float3 worldPos)
{
    float dist = distance(lightPosition, worldPos);
    float att = saturate(1.0f - (dist * dist / (lightRange * lightRange)));
    return att * att;
}

float4 DiffuseAndSpecularForADirectionalLight( float4 surfaceColor, float3 normalVector, float3 directionFromLight, float3 lightColor, float3 cameraPosition, float3 worldPosition, float roughness, float lightIntensity, float specularMapValue)
{
    normalVector = normalize(normalVector);
    
    float3 normalizedDirectionToLight = normalize(-directionFromLight);
    float3 diffuseTerm = Diffuse(normalVector, normalizedDirectionToLight) * lightColor * surfaceColor.xyz;
    float3 specularTerm = PhongSpecular(reflect(normalize(directionFromLight), normalVector), normalize(cameraPosition - worldPosition), roughness)
    * lightColor * lightIntensity;
    // Cut the specular if the diffuse contribution is zero
    // - any() returns 1 if any component of the param is non-zero
    // - In this case, diffuse is a single float value
    // - Meaning any() returns 1 if diffuse itself is non-zero
    // - In other words:
    // - If the diffuse amount is 0, any(diffuse) returns 0
    // - If the diffuse amount is != 0, any(diffuse) returns 1
    // - So when diffuse is 0, specular becomes 0
    specularTerm *= any(diffuseTerm);

    float4 totalLight = float4(diffuseTerm, 1) + float4(specularTerm * specularMapValue, 1);
    return totalLight;
}

float4 DiffuseAndSpecularForAPointLight(float4 surfaceColor, float3 normalVector, float3 directionFromLight, float3 lightColor, float3 cameraPosition, float3 worldPosition, float roughness, float lightIntensity, float3 lightPosition, float lightRange, float specularMapValue)
{
    normalVector = normalize(normalVector);
    
    float3 normalizedDirectionToLight = normalize(-directionFromLight);
    float3 diffuseTerm = Diffuse(normalVector, normalizedDirectionToLight) * lightColor * surfaceColor.xyz;
    float3 specularTerm = PhongSpecular(reflect(normalize(directionFromLight), normalVector), normalize(cameraPosition - worldPosition), roughness)
    * lightColor * lightIntensity;
    // Cut the specular if the diffuse contribution is zero
    // - any() returns 1 if any component of the param is non-zero
    // - In this case, diffuse is a single float value
    // - Meaning any() returns 1 if diffuse itself is non-zero
    // - In other words:
    // - If the diffuse amount is 0, any(diffuse) returns 0
    // - If the diffuse amount is != 0, any(diffuse) returns 1
    // - So when diffuse is 0, specular becomes 0
    specularTerm *= any(diffuseTerm);
    
    float4 totalLight = (float4(diffuseTerm, 1) + float4(specularTerm * specularMapValue, 1)) * Attenuate(lightPosition, lightRange, worldPosition);
    return totalLight;
}
#endif