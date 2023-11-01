#pragma once
#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"


class Material
{
public:
	Material(
		DirectX::XMFLOAT4 colorTint,
		float roughness,
		std::shared_ptr<SimplePixelShader> pixelShader,
		std::shared_ptr<SimpleVertexShader> vertexShader
	);
	~Material();


	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	float GetRoughness();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureSRVs();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> GetSamplers();

	DirectX::XMFLOAT4 SetColorTint();
	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetRoughness(float roughness);
	void AddTextureSRV(std::string subresourceShaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV);
	void AddTextureSR(std::string samplerShaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);

private:
	DirectX::XMFLOAT4 colorTint;
	float roughness;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	// mappings from shader-side strings to C++ values
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

};

