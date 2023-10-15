#pragma once

#include <wrl/client.h>
#include "Vertex.h"
#include <d3d11.h>
#include <string>

class Mesh {
public:
	Mesh(
		Vertex* verticies, 
		unsigned int verticiesCount, 
		unsigned int* indices, 
		unsigned int indicesCount, 
		Microsoft::WRL::ComPtr<ID3D11Device> device, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext
	);

	Mesh(
		std::wstring model,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext
	);

	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer>  GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>  GetIndexBuffer();
	int GetIndexCount();
	void Draw();

private:
	void Init(
		Vertex* verticies,
		unsigned int verticiesCount,
		unsigned int* indices,
		unsigned int indicesCount,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext
		);
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Vertex* verticies;
	unsigned int verticiesCount;
	unsigned int* indices;
	unsigned int indicesCount;
	int indexBufferCount;
};