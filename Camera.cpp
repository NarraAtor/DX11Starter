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

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
}

void Camera::UpdateViewMatrix()
{
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();

	if (input.KeyDown('W')) { /* Do something useful */ }
	if (input.KeyDown('S')) { /* Do something useful */ }

	if (input.KeyDown('A')) { /* Do something useful */ }
	if (input.KeyDown('D')) { /* Do something useful */ }

	if (input.KeyDown(VK_SPACE)) { transform.MoveAbsolute(0, 1, 0); }
	if (input.KeyDown('X')) { transform.MoveAbsolute(0, -1, 0); }

}
