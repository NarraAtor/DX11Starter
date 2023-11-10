#include "Sky.h"

Sky::Sky(
	std::shared_ptr<Mesh> mesh, 
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, 
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	std::shared_ptr<SimpleVertexShader> vertexShaderSky,
	std::shared_ptr<SimplePixelShader> pixelShaderSky,
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> skyFaces
):
	geometryMesh(mesh),
	sampleState(sampler),
	vertexShader(vertexShaderSky),
	pixelShader(pixelShaderSky)
{
	// TODO: handle texture loading here
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_FRONT;

	device->CreateRasterizerState(&rasterDesc, rasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC stencilDesc = {};
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&stencilDesc, depthState.GetAddressOf());
}

Sky::~Sky()
{
}
