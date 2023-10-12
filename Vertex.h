#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Normal;	    // The normal of the vertex
	DirectX::XMFLOAT4 Color;        // The color of the vertex
	DirectX::XMFLOAT2 UV;			// The UV of the vertex

};