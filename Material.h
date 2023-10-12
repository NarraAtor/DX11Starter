#pragma once
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"


class Material
{
public:
	Material(
		DirectX::XMFLOAT4 colorTint,
		std::shared_ptr<SimplePixelShader> pixelShader,
		std::shared_ptr<SimpleVertexShader> vertexShader
	);
	~Material();


	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();

	DirectX::XMFLOAT4 SetColorTint();
	std::shared_ptr<SimplePixelShader> SetPixelShader();
	std::shared_ptr<SimpleVertexShader> SetVertexShader();

private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
};

