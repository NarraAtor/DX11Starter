#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>

class GameEntity
{
public:
	GameEntity(shared_ptr<Mesh> mesh);
	~GameEntity();
	std::shared_ptr<Mesh> GetMesh();

private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
};

