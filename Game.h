#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include <memory>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include <vector>
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"
#include "Sky.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	void FeedInputsToImGui(float deltaTime);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> pixelShaderNormalMapping;
	std::shared_ptr<SimplePixelShader> pixelShaderSky;
	std::shared_ptr<SimplePixelShader> customPixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimpleVertexShader> vertexShaderNormalMapping;
	std::shared_ptr<SimpleVertexShader> vertexShaderSky;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	/*std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> square;
	std::shared_ptr<Mesh> diamond;*/
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> quad;
	std::shared_ptr<Mesh> quad_double_sided;
	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> torus;


	std::vector<GameEntity> gameEntities;

	// Materials
	std::vector<std::shared_ptr<Material>> materials;

	// controls to edit screen here:
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4X4 worldMatrix;

	//create camera
	std::vector<std::shared_ptr<Camera>> cameras;
	size_t currentCameraIndex;

	DirectX::XMFLOAT3 ambientColor;

	// light
	Light directionalLight0;
	Light directionalLight1;
	Light directionalLight2;
	std::vector<Light> directionalLights;
	std::vector<Light> pointLights;

	// texture stuff
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSubresources;
	std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplerStates;

	// skybox stuff
	Sky skybox;
};

