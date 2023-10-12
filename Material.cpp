#include "Material.h"

Material::Material(
    DirectX::XMFLOAT4 colorTint,
    std::shared_ptr<SimplePixelShader> pixelShader,
    std::shared_ptr<SimpleVertexShader> vertexShader
)
    :
    colorTint(colorTint),
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

