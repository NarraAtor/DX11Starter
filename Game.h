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
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

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

	// post process stuff
	// Resources that are shared among all post processes
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;
	// Resources that are tied to a particular post process
	std::shared_ptr<SimplePixelShader> ppPS;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; // For rendering
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; // For sampling
	int blurRadius;



};

