#include "Camera.h"

using namespace DirectX;
Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, float movementSpeed, float mouseLookSpeed, float fieldOfViewInRadians):
	aspectRatio(aspectRatio),
	movementSpeed(movementSpeed),
	mouseLookSpeed(mouseLookSpeed),
	fieldOfViewInRadians(fieldOfViewInRadians)
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

void Camera::UpdateProjectionMatrix(float aspectRatio, float fov)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		fieldOfViewInRadians,
		aspectRatio,
		//TODO: change these to the parameters I set
		0.01f, //near clip distance
		10000.0f // far clip distance
	);
	XMStoreFloat4x4(&projectionMatrix, proj);
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 pos = transform.GetPosition();
	XMFLOAT3 fwd = transform.GetForward();

	XMMATRIX view = XMMatrixLookToLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&fwd),
		XMVectorSet(0, 1, 0, 0)
	);

	XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();

	if (input.KeyDown('W')) { /* Do something useful */ }
	if (input.KeyDown('S')) { /* Do something useful */ }

	if (input.KeyDown('A')) { /* Do something useful */ }
	if (input.KeyDown('D')) { /* Do something useful */ }

	//TODO: multiply these by delta time
	if (input.KeyDown(VK_SPACE)) { transform.MoveAbsolute(0, 1, 0); }
	if (input.KeyDown('X')) { transform.MoveAbsolute(0, -1, 0); }

}
