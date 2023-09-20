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
