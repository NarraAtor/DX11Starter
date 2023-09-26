#include "Camera.h"

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition)
{
	transform.SetPosition(initialPosition);
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
