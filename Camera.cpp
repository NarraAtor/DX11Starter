#include "Camera.h"
#include <stdio.h>

using namespace DirectX;
Camera::Camera(
	float aspectRatio,
	DirectX::XMFLOAT3 initialPosition,
	float movementSpeed,
	float mouseLookSpeed,
	float fieldOfViewInRadians) :
	aspectRatio(aspectRatio),
	movementSpeed(movementSpeed),
	mouseLookSpeed(mouseLookSpeed),
	fieldOfViewInRadians(fieldOfViewInRadians),
	nearClipDistance(0.01f),
	farClipDistance(1000.0f)

{
	transform.SetPosition(initialPosition);

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio, fieldOfViewInRadians);
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

Transform Camera::GetTransform()
{
	return transform;
}

void Camera::UpdateProjectionMatrix(float aspectRatio, float fov)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		fieldOfViewInRadians,
		aspectRatio,
		nearClipDistance,
		farClipDistance
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

	if (input.KeyDown('W')) { transform.MoveRelative(0, 0, movementSpeed * dt); }
	if (input.KeyDown('S')) { transform.MoveRelative(0, 0, -movementSpeed * dt); }

	if (input.KeyDown('A')) { transform.MoveRelative(-movementSpeed * dt, 0, 0); }
	if (input.KeyDown('D')) { transform.MoveRelative(movementSpeed * dt, 0, 0); }

	if (input.KeyDown(' ')) {
		transform.MoveAbsolute(0, movementSpeed * dt, 0);
		//printf("pressed space");
	}
	if (input.KeyDown('X')) {
		transform.MoveAbsolute(0, -movementSpeed * dt, 0);
		//printf("pressed x");
	}

	if (input.MouseLeftDown())
	{
		float cursorMovementX = mouseLookSpeed * input.GetMouseXDelta();
		float cursorMovementY = mouseLookSpeed * input.GetMouseYDelta();
		transform.Rotate(cursorMovementY, cursorMovementX, 0);
	}

	UpdateViewMatrix();
}
