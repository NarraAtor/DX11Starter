#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include <memory>
#include "SimpleShader.h"
#include "Camera.h"


class Sky
{

public:
	//giving this a default constructor since the game class wants that in its constructor
	Sky();
	Sky(std::shared_ptr<Mesh> mesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		std::shared_ptr<SimpleVertexShader> vertexShaderSky,
		std::shared_ptr<SimplePixelShader> pixelShaderSky,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyFaces
	);
		~Sky();
		void Draw(
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context,
			std::shared_ptr<Camera> camera);
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSubresourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	std::shared_ptr<Mesh> geometryMesh;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
};

