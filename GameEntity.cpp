#include "GameEntity.h"

using namespace std;
GameEntity::GameEntity(shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
	mesh(mesh),
	transform(),
	material(material)
{
	
}

GameEntity::~GameEntity()
{
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

Transform* GameEntity::GetTransform()
{
	return &transform;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}
