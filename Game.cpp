#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "BufferStructs.h"
#include "SimpleShader.h"


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
	materials.push_back(std::make_shared<Material>(XMFLOAT4(255, 0, 0, 1), pixelShader, vertexShader));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0, 255, 0, 1), pixelShader, vertexShader));
	materials.push_back(std::make_shared<Material>(XMFLOAT4(0, 0, 255, 1), pixelShader, vertexShader));

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
				1.0f,
				XM_PIDIV4));

		cameras.push_back(
			std::make_shared<Camera>(
				(float)windowWidth / windowHeight,
				XMFLOAT3(5.0f, 0.0f, -5.0f),
				5.0f,
				1.0f,
				XM_PIDIV4));
		cameras.push_back(
			std::make_shared<Camera>(
				(float)windowWidth / windowHeight,
				XMFLOAT3(0.0f, 5.0f, -5.0f),
				5.0f,
				1.0f,
				XM_PIDIV4));
	}

	currentCameraIndex = 0;

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
	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader.cso").c_str());
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
	Vertex triangleVertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};

	Vertex squareVertices[] =
	{
		{ XMFLOAT3(+0.5f, +0.8f, +0.0f), red },
		{ XMFLOAT3(+0.9f, -0.2f, +0.0f), red },
		{ XMFLOAT3(0.5f, -0.2f, +0.0f), red },
		{ XMFLOAT3(0.9f, 0.8f, +0.0f), red },
		{ XMFLOAT3(+0.9f, -0.2f, +0.0f), red },
		{ XMFLOAT3(+0.5f, +0.8f, +0.0f), red }
	};

	Vertex diamondVertices[] =
	{
		{ XMFLOAT3(-0.9f, +0.3f, +0.0f), red },
		{ XMFLOAT3(-0.5f, 0.3f, +0.0f), red },
		{ XMFLOAT3(-0.7f, -0.2f, +0.0f), red },
		{ XMFLOAT3(-0.9f, +0.3f, +0.0f), blue },
		{ XMFLOAT3(-0.7f, 0.8f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, 0.3f, +0.0f), blue },
	};


	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int triangleIndices[] = { 0, 1, 2 };
	unsigned int squareIndices[] = { 0, 1, 2, 3, 4, 5 };
	unsigned int diamondIndices[] = { 0, 1, 2, 3, 4, 5 };



	triangle = std::make_shared<Mesh>(triangleVertices, 3, triangleIndices, 3, device, context);
	square = std::make_shared<Mesh>(squareVertices, 6, squareIndices, 6, device, context);
	diamond = std::make_shared<Mesh>(diamondVertices, 6, diamondIndices, 6, device, context);

	gameEntities.push_back(GameEntity(triangle, materials[0]));
	gameEntities.push_back(GameEntity(square, materials[1]));
	gameEntities.push_back(GameEntity(diamond, materials[2]));
	gameEntities.push_back(GameEntity(square, materials[0]));
	gameEntities.push_back(GameEntity(square, materials[1]));


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
	ImGui::Text("Framerate: %f", ImGui::GetIO().Framerate);
	ImGui::Text("Window Width: %lu", windowWidth);
	ImGui::Text("Window Height: %lu", windowHeight);


	// controls to edit screen here:

	//ImGui::DragFloat3("Edit a vector", &offset.x);
	ImGui::ColorEdit4("4 - component(RGBA) color editor", &color.x);


	gameEntities[0].GetTransform()->Rotate(0, 0, 0.0001f);
	gameEntities[1].GetTransform()->Rotate(0, 0, 0.0001f);
	gameEntities[2].GetTransform()->Rotate(0, 0, 0.0001f);
	gameEntities[3].GetTransform()->Rotate(0, 0, -0.0001f);
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


	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();


}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
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
		vs->SetFloat4("colorTint", entity.GetMaterial().get()->GetColorTint());
		vs->SetMatrix4x4("world", entity.GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("view", cameras[currentCameraIndex]->GetViewMatrix());
		vs->SetMatrix4x4("proj", cameras[currentCameraIndex]->GetProjectionMatrix());

		vs->CopyAllBufferData(); // Adjust “vs” variable name if necessary

		entity.GetMaterial().get()->GetVertexShader().get()->SetShader();
		entity.GetMaterial().get()->GetPixelShader().get()->SetShader();

		entity.GetMesh().get()->Draw();
	}

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