#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include <memory>
#include "SimpleShader.h"


class Sky
{

public:
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Sky();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSubresourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	std::shared_ptr<Mesh> geometryMesh;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
};

