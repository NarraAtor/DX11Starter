#include "Material.h"

Material::Material(
    DirectX::XMFLOAT4 colorTint,
    float roughness,
    std::shared_ptr<SimplePixelShader> pixelShader,
    std::shared_ptr<SimpleVertexShader> vertexShader
)
    :
    colorTint(colorTint),
    roughness(roughness),
    pixelShader(pixelShader),
    vertexShader(vertexShader)
{
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

float Material::GetRoughness()
{
    return roughness;
}

DirectX::XMFLOAT4 Material::SetColorTint()
{
    return colorTint;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader)
{
    this->pixelShader = pixelShader;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader)
{
    this->vertexShader = vertexShader;
}

void Material::SetRoughness(float roughness)
{
    this->roughness = roughness;
}

void Material::AddTextureSRV(std::string subresourceShaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV)
{
    textureSRVs.insert({ subresourceShaderName, textureSRV });
}

void Material::AddTextureSR(std::string samplerShaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers.insert({ samplerShaderName, sampler });
}

