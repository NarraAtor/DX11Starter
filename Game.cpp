#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "BufferStructs.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"


// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	// intializations placed here to deal with editor warnings
	color = XMFLOAT4(0, 0, 0, 0);
	currentCameraIndex = 0;
	worldMatrix = XMFLOAT4X4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	directionalLight0 = {};
	directionalLight1 = {};
	directionalLight2 = {};
	lightViewMatrix = XMMATRIX();
	lightProjectionMatrix = XMMATRIX();
	shadowViewMatrix = XMFLOAT4X4();
	shadowProjectionMatrix = XMFLOAT4X4();
	shadowMapResolution = 1024;

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	textureSubresources.push_back(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>());
	textureSubresources.push_back(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>());
	textureSubresources.push_back(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>());
	textureSubresources.push_back(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>());
	textureSubresources.push_back(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>());
	textureSubresources.push_back(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>());



	//CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cushion.png").c_str(), nullptr, textureSubresources[0].GetAddressOf());
	//CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cushion_normals.png").c_str(), nullptr, textureSubresources[1].GetAddressOf());
	//CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/TCom_Gore_512_albedo.tif").c_str(), nullptr, textureSubresources[0].GetAddressOf());
	//CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/TCom_Gore_512_ao.tif").c_str(), nullptr, textureSubresources[1].GetAddressOf());
	//CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/TCom_Gore_512_normal.tif").c_str(), nullptr, textureSubresources[2].GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/bronze_albedo.png").c_str(), nullptr, textureSubresources[0].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/bronze_metal.png").c_str(), nullptr, textureSubresources[1].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/bronze_normals.png").c_str(), nullptr, textureSubresources[2].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/bronze_roughness.png").c_str(), nullptr, textureSubresources[3].GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/tiles.png").c_str(), nullptr, textureSubresources[4].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/tiles_specular.png").c_str(), nullptr, textureSubresources[5].GetAddressOf());

	samplerStates.push_back(Microsoft::WRL::ComPtr<ID3D11SamplerState>());
	D3D11_SAMPLER_DESC sampleDescription0 = {};

	sampleDescription0.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // Each dimension can
	sampleDescription0.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; // have a different mode
	sampleDescription0.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // but that is uncommon
	sampleDescription0.Filter = D3D11_FILTER_ANISOTROPIC;
	sampleDescription0.MaxAnisotropy = 16;
	sampleDescription0.MaxLOD = D3D11_FLOAT32_MAX; // Maximum mip level
	device->CreateSamplerState(&sampleDescription0, samplerStates[0].GetAddressOf());

	// Change this back to the standard pixel and vertex shader
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 0, 0, 1), 0.75f, pixelShaderNormalMapping, vertexShaderNormalMapping));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0, 1, 0, 1), 0.5f, pixelShader, vertexShader));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0, 0, 1, 1), 0.01f, pixelShader, vertexShader));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(1, 0, 1, 0.5f), 0.5f, customPixelShader, vertexShader));

	materials[0].get()->AddTextureSRV("Albedo", textureSubresources[0]);
	materials[0].get()->AddTextureSRV("MetalnessMap", textureSubresources[1]);

	// TODO: Find/create a specular map?
	// materials[0].get()->AddTextureSRV("SpecularTexture", textureSubresources[1]);
	materials[0].get()->AddTextureSRV("NormalMap", textureSubresources[2]);
	materials[0].get()->AddTextureSRV("RoughnessMap", textureSubresources[3]);

	materials[0].get()->AddTextureSR("BasicSampler", samplerStates[0]);


	materials[1].get()->AddTextureSRV("Albedo", textureSubresources[4]);
	materials[1].get()->AddTextureSRV("SpecularTexture", textureSubresources[5]);
	materials[1].get()->AddTextureSR("BasicSampler", samplerStates[0]);

	CreateGeometry();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		/*context->
		context->*/


		// Initialize ImGui itself & platform/renderer backends
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(device.Get(), context.Get());
		// Pick a style (uncomment one of these 3)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();

		// create constant buffer
		// Get size as the next multiple of 16 (instead of hardcoding a size here!)
		unsigned int size = sizeof(VertexShaderExternalData);
		size = (size + 15) / 16 * 16; // This will work even if the struct size changes

		// Describe the constant buffer
		D3D11_BUFFER_DESC cbDesc = {}; // Sets struct to all zeros
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = size; // Must be a multiple of 16
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;

		// set up editable features
		color = XMFLOAT4(1.0f, 0.0f, 0.5f, 1.0f);
		worldMatrix = XMFLOAT4X4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		cameras.push_back(
			std::make_shared<Camera>(
				(float)windowWidth / windowHeight,
				XMFLOAT3(0.0f, 0.0f, -5.0f),
				5.0f,
				0.01f,
				XM_PIDIV4));

		cameras.push_back(
			std::make_shared<Camera>(
				(float)windowWidth / windowHeight,
				XMFLOAT3(5.0f, 0.0f, -5.0f),
				5.0f,
				0.01f,
				XM_PIDIV4));
		cameras.push_back(
			std::make_shared<Camera>(
				(float)windowWidth / windowHeight,
				XMFLOAT3(0.0f, 5.0f, -5.0f),
				5.0f,
				0.01f,
				XM_PIDIV4));
	}

	currentCameraIndex = 0;
	directionalLight0.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight0.Direction = XMFLOAT3(0, -1, 0);
	directionalLight0.Color = XMFLOAT3(1, 1, 1);
	directionalLight0.Intensity = 1.0f;
	directionalLights.push_back(directionalLight0);

	directionalLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.Direction = XMFLOAT3(1, 0, 0);
	directionalLight1.Color = XMFLOAT3(1, 0, 1);
	directionalLight1.Intensity = 1.0f;
	directionalLights.push_back(directionalLight1);

	directionalLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = XMFLOAT3(0, 0, 1);
	directionalLight2.Color = XMFLOAT3(0, 1, 0);
	directionalLight2.Intensity = 0.75f;
	directionalLights.push_back(directionalLight2);

	pointLights.push_back(Light());
	pointLights[0] = {};
	pointLights[0].Type = LIGHT_TYPE_POINT;
	pointLights[0].Color = XMFLOAT3(1, 1, 1);
	pointLights[0].Intensity = 1.0f;
	pointLights[0].Position = XMFLOAT3(7.5f, 0.0f, 0.0f);
	pointLights[0].Range = 10.0f;

	pointLights.push_back(Light());
	pointLights[1] = {};
	pointLights[1].Type = LIGHT_TYPE_POINT;
	pointLights[1].Color = XMFLOAT3(0, 0, 1);
	pointLights[1].Intensity = 1.0f;
	pointLights[1].Position = XMFLOAT3(2.5f, 0.0f, 0.0f);
	pointLights[1].Range = 10.0f;


	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.Height = shadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());
	
	int offset = 80;
	lightViewMatrix = XMMatrixLookToLH(
		XMVectorSet(5, -directionalLight2.Direction.y * offset, -directionalLight2.Direction.z * offset, 1.0f), // Position: "Backing up" 20 units from origin
		//XMVectorSet(0.0f, 20.0f, 10.0f, 1.0f),
		XMVectorSet(directionalLight2.Direction.x, directionalLight2.Direction.y , directionalLight2.Direction.z, 1.0f), // Direction: light's direction
		XMVectorSet(0, 1, 0, 0)); // Up: World up vector (Y axis)

	float lightProjectionSize = 15.0f; // Tweak for your scene!
	lightProjectionMatrix = XMMatrixOrthographicLH(
		lightProjectionSize,
		lightProjectionSize,
		1.0f,
		100.0f);

	XMStoreFloat4x4(&shadowViewMatrix, lightViewMatrix);
	XMStoreFloat4x4(&shadowProjectionMatrix, lightProjectionMatrix);

}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader.cso").c_str());
	vertexShaderNormalMapping = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShaderWithNormalMaps.cso").c_str());
	vertexShaderSky = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShaderSkybox.cso").c_str());
	shadowVS = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"ShadowMapVertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader.cso").c_str());
	pixelShaderNormalMapping = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShaderWithNormalMaps.cso").c_str());
	pixelShaderSky = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShaderSkybox.cso").c_str());
	customPixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"CustomPS.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	// Vertex triangleVertices[] =
	// {
	// 	{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
	// 	{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
	// 	{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	// };
	// 
	// Vertex squareVertices[] =
	// {
	// 	{ XMFLOAT3(+0.5f, +0.8f, +0.0f), red },
	// 	{ XMFLOAT3(+0.9f, -0.2f, +0.0f), red },
	// 	{ XMFLOAT3(0.5f, -0.2f, +0.0f), red },
	// 	{ XMFLOAT3(0.9f, 0.8f, +0.0f), red },
	// 	{ XMFLOAT3(+0.9f, -0.2f, +0.0f), red },
	// 	{ XMFLOAT3(+0.5f, +0.8f, +0.0f), red }
	// };
	// 
	// Vertex diamondVertices[] =
	// {
	// 	{ XMFLOAT3(-0.9f, +0.3f, +0.0f), red },
	// 	{ XMFLOAT3(-0.5f, 0.3f, +0.0f), red },
	// 	{ XMFLOAT3(-0.7f, -0.2f, +0.0f), red },
	// 	{ XMFLOAT3(-0.9f, +0.3f, +0.0f), blue },
	// 	{ XMFLOAT3(-0.7f, 0.8f, +0.0f), blue },
	// 	{ XMFLOAT3(-0.5f, 0.3f, +0.0f), blue },
	// };


	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	//unsigned int triangleIndices[] = { 0, 1, 2 };
	//unsigned int squareIndices[] = { 0, 1, 2, 3, 4, 5 };
	//unsigned int diamondIndices[] = { 0, 1, 2, 3, 4, 5 };



	//triangle = std::make_shared<Mesh>(triangleVertices, 3, triangleIndices, 3, device, context);
	//square = std::make_shared<Mesh>(squareVertices, 6, squareIndices, 6, device, context);
	//diamond = std::make_shared<Mesh>(diamondVertices, 6, diamondIndices, 6, device, context);

	cube = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.objectFile").c_str(), device, context);
	cylinder = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cylinder.objectFile").c_str(), device, context);
	helix = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/helix.objectFile").c_str(), device, context);
	quad = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad.objectFile").c_str(), device, context);
	quad_double_sided = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad_double_sided.objectFile").c_str(), device, context);
	sphere = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.objectFile").c_str(), device, context);
	torus = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/torus.objectFile").c_str(), device, context);
	/*square = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.objectFile").c_str(), device);
	diamond = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.objectFile").c_str(), device);*/

	/*gameEntities.push_back(GameEntity(triangle, materials[0]));
	gameEntities.push_back(GameEntity(square, materials[1]));
	gameEntities.push_back(GameEntity(diamond, materials[2]));
	gameEntities.push_back(GameEntity(square, materials[0]));
	gameEntities.push_back(GameEntity(square, materials[1]));*/

	gameEntities.push_back(GameEntity(cube, materials[1]));
	gameEntities.push_back(GameEntity(cylinder, materials[0]));
	gameEntities[1].GetTransform()->MoveAbsolute(XMFLOAT3(5.0f, 0.0f, 0.0f));

	gameEntities.push_back(GameEntity(helix, materials[1]));
	gameEntities[2].GetTransform()->MoveAbsolute(XMFLOAT3(10.0f, 0.0f, 0.0f));

	gameEntities.push_back(GameEntity(quad, materials[1]));
	gameEntities[3].GetTransform()->MoveAbsolute(XMFLOAT3(15.0f, -2.0f, 2.0f));
	gameEntities[3].GetTransform()->Scale(XMFLOAT3(20.0f, 20.0f, 20.0f));
	gameEntities[3].GetTransform()->Rotate(31.0f, 0.0f, 0.0f);



	gameEntities.push_back(GameEntity(quad_double_sided, materials[2]));
	gameEntities[4].GetTransform()->MoveAbsolute(XMFLOAT3(20.0f, 0.0f, 0.0f));

	gameEntities.push_back(GameEntity(sphere, materials[2]));
	gameEntities[5].GetTransform()->MoveAbsolute(XMFLOAT3(25.0f, 0.0f, 0.0f));

	gameEntities.push_back(GameEntity(torus, materials[3]));
	gameEntities[6].GetTransform()->MoveAbsolute(XMFLOAT3(30.0f, 0.0f, 0.0f));


	/*gameEntities.push_back(GameEntity(square, materials[1]));
	gameEntities.push_back(GameEntity(diamond, materials[2]));
	gameEntities.push_back(GameEntity(square, materials[0]));
	gameEntities.push_back(GameEntity(square, materials[1]));*/

	skybox = Sky(
		cube,
		samplerStates[0],
		device,
		vertexShaderSky,
		pixelShaderSky,
		CreateCubemap(
			FixPath(L"../../Assets/Textures/right.png").c_str(),
			FixPath(L"../../Assets/Textures/left.png").c_str(),
			FixPath(L"../../Assets/Textures/up.png").c_str(),
			FixPath(L"../../Assets/Textures/down.png").c_str(),
			FixPath(L"../../Assets/Textures/front.png").c_str(),
			FixPath(L"../../Assets/Textures/back.png").c_str()
			)
		);
}

void Game::FeedInputsToImGui(float deltaTime)
{
	// Feed fresh input data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input& input = Input::GetInstance();
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	ImGui::ShowDemoWindow();
}



// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	for (std::shared_ptr<Camera> camera : cameras)
	{
		cameras[currentCameraIndex]->UpdateProjectionMatrix((float)windowWidth / windowHeight, XM_PIDIV4);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{

	FeedInputsToImGui(deltaTime);

	ImGui::Image(shadowSRV.Get(), ImVec2(512, 512));

	ImGui::Text("Framerate: %f", ImGui::GetIO().Framerate);
	ImGui::Text("Window Width: %lu", windowWidth);
	ImGui::Text("Window Height: %lu", windowHeight);


	// controls to edit screen here:

	//ImGui::DragFloat3("Edit a vector", &offset.x);
	ImGui::ColorEdit4("4 - component(RGBA) color editor", &color.x);


	gameEntities[0].GetTransform()->Rotate(0, 0, 0.0001f);
	 gameEntities[1].GetTransform()->Rotate(0, 0, 0.0001f);
	 gameEntities[2].GetTransform()->Rotate(0, 0, 0.0001f);
	 // gameEntities[3].GetTransform()->Rotate(0, 0, -0.0001f);
	 gameEntities[4].GetTransform()->MoveAbsolute(-0.000001f, 0, 0);

	// entity UI data here
	for (int i = 0; i < gameEntities.size(); i++)
	{
		// for drop down if I ever feel like it
		// ImGui::ListBox("Scene Entities", )

		ImGui::Text("Entity %d", i);
		XMFLOAT3 position = gameEntities[i].GetTransform()->GetPosition();
		XMFLOAT3 scale = gameEntities[i].GetTransform()->GetScale();
		XMFLOAT3 rotation = gameEntities[i].GetTransform()->GetPitchYawRoll();

		ImGui::DragFloat3("Position##%f", &position.x, i * 1.0f);
		ImGui::DragFloat3("Scale##f", &scale.x, i * 1.0f);
		ImGui::DragFloat3("Rotation in radians##%f", &rotation.x, i * 1.0f);
		ImGui::Text("Mesh Index Count: %d", i, i);
	}

	XMFLOAT3 cameraPosition = cameras[currentCameraIndex]->GetTransform().GetPosition();

	ImGui::Text("\nCurrent Camera Position: %f, %f, %f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	ImGui::Text("Current Camera FOV: %f", cameras[currentCameraIndex]->getFOV());


	if (ImGui::Button("Previous"))
	{
		if (currentCameraIndex == 0)
		{
			currentCameraIndex = cameras.size() - 1;
		}
		else
		currentCameraIndex = currentCameraIndex - 1;
		
	}

	if (ImGui::Button("Next"))
	{
		currentCameraIndex = (currentCameraIndex + 1) % cameras.size();
	}
	

	
	cameras[currentCameraIndex]->Update(deltaTime);

	// light UI stuff here

	for (int i = 0; i < directionalLights.size(); i++)
	{
		if (i < directionalLights.size())
		{
			const size_t possibleChangeToBufferSizeIfIGetsBig = std::to_string(i).length();
			char* directionalLightTextHeap = new char[21 + possibleChangeToBufferSizeIfIGetsBig];
			snprintf(directionalLightTextHeap, 21 + possibleChangeToBufferSizeIfIGetsBig, "Directional Light %d", i);
			const char* directionalLightTextConstant = directionalLightTextHeap;

			ImGui::ColorEdit4(directionalLightTextConstant, &directionalLights[i].Color.x, ImGuiColorEditFlags_DefaultOptions_);
			delete[] directionalLightTextHeap;
			directionalLightTextHeap = nullptr;
		}

	}

	for (int i = 0; i < pointLights.size(); i++)
	{
		if (i < pointLights.size())
		{
			const size_t possibleChangeToBufferSizeIfIGetsBig = std::to_string(i).length();
			char* pointLightTextHeap = new char[15 + possibleChangeToBufferSizeIfIGetsBig];
			snprintf(pointLightTextHeap, 15 + possibleChangeToBufferSizeIfIGetsBig, "Point Light %d", i);
			const char* pointLightTextConstant = pointLightTextHeap;

			ImGui::ColorEdit4(pointLightTextConstant, &pointLights[i].Color.x, ImGuiColorEditFlags_DefaultOptions_);
			delete[] pointLightTextHeap;
			pointLightTextHeap = nullptr;
		}

	}

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();


}

// --------------------------------------------------------
// Loads six individual textures (the six faces of a cube map), then
// creates a blank cube map and copies each of the six textures to
// another face. Afterwards, creates a shader resource view for
// the cube map and cleans up all of the temporary resources.
// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not SHADER RESOURCE VIEWS!
	// - Explicitly NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here! +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};
	CreateWICTextureFromFile(device.Get(), right, (ID3D11Resource**)textures[0].GetAddressOf(), 0);
	CreateWICTextureFromFile(device.Get(), left, (ID3D11Resource**)textures[1].GetAddressOf(), 0);
	CreateWICTextureFromFile(device.Get(), up, (ID3D11Resource**)textures[2].GetAddressOf(), 0);
	CreateWICTextureFromFile(device.Get(), down, (ID3D11Resource**)textures[3].GetAddressOf(), 0);
	CreateWICTextureFromFile(device.Get(), front, (ID3D11Resource**)textures[4].GetAddressOf(), 0);
	CreateWICTextureFromFile(device.Get(), back, (ID3D11Resource**)textures[5].GetAddressOf(), 0);
	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first texture
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);
	// Describe the resource for the cube map, which is simply
	// a "texture 2d array" with the TEXTURECUBE flag set.
	// This is a special GPU resource format, NOT just a
	// C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width; // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // A CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;
	// Create the final texture resource to hold the cube map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	device->CreateTexture2D(&cubeDesc, 0, cubeMapTexture.GetAddressOf());
	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0, // Which mip (zero, since there's only one)
			i, // Which array element?
			1); // How many mip levels are in the texture?
		// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture.Get(), // Destination resource
			subresource, // Dest subresource index (one of the array elements)
			0, 0, 0, // XYZ location of copy
			textures[i].Get(), // Source resource
			0, // Source subresource index (we're assuming there's only one)
			0); // Source subresource "box" of data to copy (zero means the whole thing)
	}
	// At this point, all of the faces have been copied into the
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1; // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see
	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());
	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// shadow map stuff
	context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV{};
	context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());

	context->PSSetShader(0, 0, 0);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)shadowMapResolution;
	viewport.Height = (float)shadowMapResolution;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", shadowViewMatrix);
	shadowVS->SetMatrix4x4("projection", shadowProjectionMatrix);

	// Loop and draw all entities
	for (GameEntity entity : gameEntities)
	{
		shadowVS->SetMatrix4x4("world", entity.GetTransform()->GetWorldMatrix());
		shadowVS->SetMatrix4x4("lightView", entity.GetTransform()->GetWorldMatrix());
		shadowVS->SetMatrix4x4("lightProjection", entity.GetTransform()->GetWorldMatrix());

		// handles map here
		//shadowVS->SetShaderResourceView("ShadowMap", shadowSRV);

		shadowVS->CopyAllBufferData();
		// Draw the mesh directly to avoid the entity's material
		entity.GetMesh().get()->Draw();
	}

	viewport.Width = (float)this->windowWidth;
	viewport.Height = (float)this->windowHeight;
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(
		1,
		backBufferRTV.GetAddressOf(),
		depthBufferDSV.Get());

	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	for (GameEntity entity : gameEntities)
	{
		std::shared_ptr<SimpleVertexShader> vs = entity.GetMaterial().get()->GetVertexShader();
		vs->SetMatrix4x4("world", entity.GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("view", cameras[currentCameraIndex]->GetViewMatrix());
		vs->SetMatrix4x4("proj", cameras[currentCameraIndex]->GetProjectionMatrix());
		vs->SetMatrix4x4("worldInvTranspose", entity.GetTransform()->GetWorldInverseTransposeMatrix());
		vs->SetMatrix4x4("lightView", entity.GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("lightProjection", entity.GetTransform()->GetWorldMatrix());


		vs->CopyAllBufferData(); // Adjust “vs” variable name if necessary

		std::shared_ptr<SimplePixelShader> ps = entity.GetMaterial().get()->GetPixelShader();
		ps->SetFloat4("colorTint", entity.GetMaterial().get()->GetColorTint());
		ps->SetFloat("roughness", entity.GetMaterial().get()->GetRoughness());
		ps->SetFloat3("cameraPosition", cameras[currentCameraIndex].get()->GetTransform().GetPosition());
		ps->SetData(
			"directionalLights", // The name of the (eventual) variable in the shader
			&directionalLights[0], // The address of the data to set
			sizeof(Light) * (int)directionalLights.size()); // The size of the data (the whole array!) to set
		ps->SetData(
			"pointLights", // The name of the (eventual) variable in the shader
			&pointLights[0], // The address of the data to set
			sizeof(Light) * (int)pointLights.size()); // The size of the data (the whole array!) to set
		ps->SetMatrix4x4("lightView", entity.GetTransform()->GetWorldMatrix());
		ps->SetMatrix4x4("lightProjection", entity.GetTransform()->GetWorldMatrix());

		// handles textures here
		for (auto& t : entity.GetMaterial().get()->GetTextureSRVs()) { ps->SetShaderResourceView(t.first.c_str(), t.second); }
		for (auto& s : entity.GetMaterial().get()->GetSamplers()) { ps->SetSamplerState(s.first.c_str(), s.second); }

		// SHADOW MAP
		ps->SetShaderResourceView("ShadowMap", shadowSRV);
		ps->CopyAllBufferData(); // Adjust “ps” variable name if necessary


		entity.GetMaterial().get()->GetVertexShader().get()->SetShader();
		entity.GetMaterial().get()->GetPixelShader().get()->SetShader();

		entity.GetMesh().get()->Draw();
	}

	skybox.Draw(context, cameras[currentCameraIndex]);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}