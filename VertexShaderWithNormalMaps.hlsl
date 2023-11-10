#include "NewInclude.hlsli"
// Constant buffer defined here
cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    matrix worldInvTranspose;

}

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
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel_NormalMap main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel_NormalMap output;

    matrix worldViewProjectionMatrix = mul(proj, mul(view, world));

	// Here we're essentially passing the input position directly through to the next
	// stage (rasterizer), though it needs to be a 4-component vector now.  
	// - To be considered within the bounds of the screen, the X and Y components 
	//   must be between -1 and 1.  
	// - The Z component must be between 0 and 1.  
	// - Each of these components is then automatically divided by the W component, 
	//   which we're leaving at 1.0 for now (this is more useful when dealing with 
	//   a perspective projection matrix, which we'll get to in the future).
    output.screenPosition = mul(worldViewProjectionMatrix, float4(input.localPosition, 1.0f));
	
    output.uv = input.uv;
    output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;
	// move our normal with our model (also handles non-uniform scales)
   // output.normal = mul((float3x3) world, input.normal);
    // this line shows black -> figure out a way to fix this later: 
    output.normal = mul((float3x3) worldInvTranspose, input.normal);
	
    output.tangent = mul((float3x3) world, input.tangent);

	
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
    return output;
}