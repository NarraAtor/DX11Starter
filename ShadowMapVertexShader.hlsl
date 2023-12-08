#include "NewInclude.hlsli"

// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ Position
    float3 normal : NORMAL; // Normal vectors
    float2 uv : TEXCOORD; // UV Maps
    float3 tangent : TANGENT; // Tangent of texture
};

// --------------------------------------------------------
// A simplified vertex shader for rendering to a shadow map
// --------------------------------------------------------
float4 main(VertexShaderInput input) : SV_POSITION
{
    matrix wvp = mul(projection, mul(view, world));
    return mul(wvp, float4(input.localPosition, 1.0f));
}