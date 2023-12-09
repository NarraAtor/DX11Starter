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
    float4 shadowMapPos : SHADOW_POSITION;
    float2 uv : TEXCOORD; // UV Maps
    float3 worldPosition : POSITION; // XYZ Position
    float3 normal : NORMAL; // Normal vectors
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel_Sky
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float3 sampleDir : DIRECTION; // XYZ Position
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
    float4 shadowMapPos : SHADOW_POSITION;
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
    float spec = 0;
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    if(specExponent > 0.05f)
    {
        spec = pow(saturate(dot(reflectionVector, surfaceToCameraVector)), specExponent);
    }
    return spec;
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

// CONSTANTS ===================

// Make sure to place these at the top of your shader(s) or shader include file
// - You don't necessarily have to keep all the comments; they're here for your reference

// A constant Fresnel value for non-metals (glass and plastic have values of about 0.04)
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;




// PBR FUNCTIONS ================

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}



// Calculates diffuse amount based on energy conservation
//
// diffuse   - Diffuse amount
// F         - Fresnel result from microfacet BRDF
// metalness - surface metalness amount 
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}
 


// Normal Distribution Function: GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
	// Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	// Can go to zero if roughness is 0 and NdotH is 1
    float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}



// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
    float VdotH = saturate(dot(v, h));

	// Final value
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}



// Geometric Shadowing - Schlick-GGX
// - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
//
// n - Normal
// v - View vector
//
// G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
//
// Full G(n,v,l,a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
	// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));

	// Final value
	// Note: Numerator should be NdotV (or NdotL depending on parameters).
	// However, these are also in the BRDF's denominator, so they'll cancel!
	// We're leaving them out here AND in the BRDF function as the
	// dot products can get VERY small and cause rounding errors.
    return 1 / (NdotV * (1 - k) + k);
}


 
// Cook-Torrance Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - parts of the denominator are canceled out by numerator (see below)
//
// D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
	// Other vectors
    float3 h = normalize(v + l);

	// Run numerator functions
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
	
	// Pass F out of the function for diffuse balance
    F_out = F;

	// Final specular formula
	// Note: The denominator SHOULD contain (NdotV)(NdotL), but they'd be
	// canceled out by our G() term.  As such, they have been removed
	// from BOTH places to prevent floating point rounding errors.
    float3 specularResult = (D * F * G) / 4;

	// One last non-obvious requirement: According to the rendering equation,
	// specular must have the same NdotL applied as diffuse!  We'll apply
	// that here so that minimal changes are required elsewhere.
    return specularResult * max(dot(n, l), 0);
}


#endif