#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>

class GameEntity
{
public:
	GameEntity(shared_ptr<Mesh> mesh);
	~GameEntity();

private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
};

