#pragma once
#include "Input.h"
#include "Transform.h"

class Camera
{
public:

private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	float fieldOfViewInRadians;
	float nearClipDistance;
	float farClipDistance;
	float movementSpeed;
	float mouseLookSpeed;
	bool isPerspective; // if not perspective, is assummed to be orthographic
};

