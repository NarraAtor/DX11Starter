#pragma once
#include "Transform.h"
#include "Mesh.h"
#include <memory>
#include "Material.h"

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
	~GameEntity();

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();
	Transform* GetTransform();

	void SetMaterial(std::shared_ptr<Material> material);

private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

