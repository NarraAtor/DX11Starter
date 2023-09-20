#include "GameEntity.h"

using namespace std;
GameEntity::GameEntity(shared_ptr<Mesh> mesh) :
	mesh(mesh),
	transform()
{
	
}

GameEntity::~GameEntity()
{
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

Transform* GameEntity::GetTransform()
{
	return &transform;
}