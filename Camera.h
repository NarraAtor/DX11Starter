#pragma once
#include "Input.h"
#include "Transform.h"

class Camera
{
public:
	Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, float movementSpeed, float mouseLookSpeed, float fieldOfViewInRadians);
	~Camera();

	//getters
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	Transform GetTransform();
	float getFOV();

	void UpdateProjectionMatrix(float aspectRatio, float fov);
	void UpdateViewMatrix();
	void Update(float dt);

private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	float fieldOfViewInRadians;
	float aspectRatio;
	float nearClipDistance;
	float farClipDistance;
	float movementSpeed;
	float mouseLookSpeed;
	bool isPerspective; // if not perspective, is assummed to be orthographic
};

