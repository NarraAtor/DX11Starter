#include "Transform.h"

using namespace DirectX;
Transform::Transform() :
	position(0, 0, 0),
	scale(1, 1, 1),
	pitchYawRoll(0, 0, 0),
	up(0, 1, 0),
	right(1, 0, 0),
	forward(0, 0, 1)
{
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
	matrixDirty = false;
	vectorsDirty = false;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	matrixDirty = true;

}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	SetPosition(position.x, position.y, position.z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
	matrixDirty = true;
	vectorsDirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	SetRotation(rotation.x, rotation.y, rotation.z);
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
	matrixDirty = true;

}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	SetScale(scale.x, scale.y, scale.z);
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	SetPosition(position.x + x, position.y + y, position.z + z);
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	MoveAbsolute(offset.x, offset.y, offset.z);
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR relativeMovementVector = XMVectorSet(x, y, z, 0);
	XMVECTOR positionVector = XMLoadFloat3(&position);
	XMStoreFloat3(&position,
		XMVector3Rotate(
			relativeMovementVector,
			XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z)) + positionVector);
	matrixDirty = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	SetRotation(pitchYawRoll.x + pitch, pitchYawRoll.y + yaw, pitchYawRoll.z + roll);
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	Rotate(rotation.x, rotation.y, rotation.z);
}

void Transform::Scale(float x, float y, float z)
{
	SetScale(scale.x * x, scale.y * y, scale.z * z);
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	Scale(scale.x, scale.y, scale.z);
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{

	if (matrixDirty)
	{
		// TODO: do stuff
		XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX scalingMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
		XMMATRIX worldSIMDMatrix = scalingMatrix * rotationMatrix * translationMatrix;

		XMStoreFloat4x4(&world, worldSIMDMatrix);
		XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(worldSIMDMatrix)));

		matrixDirty = false;
	}
	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (matrixDirty)
	{
		// TODO: do stuff
		XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX scalingMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
		XMMATRIX worldSIMDMatrix = scalingMatrix * rotationMatrix * translationMatrix;

		XMStoreFloat4x4(&world, worldSIMDMatrix);
		XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(worldSIMDMatrix)));

		matrixDirty = false;
	}
	return worldInverseTranspose;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	UpdateVectors();
	return up;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	UpdateVectors();
	return right;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	UpdateVectors();
	return forward;
}

void Transform::UpdateVectors()
{
	if (!vectorsDirty)
	{
		return;
	}

	XMFLOAT3 worldUp(0, 1, 0);
	XMFLOAT3 worldRight(1, 0, 0);
	XMFLOAT3 worldForward(0, 0, 1);


	XMVECTOR rotationQuaternion = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMStoreFloat3(&up, XMVector3Rotate(XMLoadFloat3(&worldUp), rotationQuaternion));
	XMStoreFloat3(&right, XMVector3Rotate(XMLoadFloat3(&worldRight), rotationQuaternion));
	XMStoreFloat3(&forward, XMVector3Rotate(XMLoadFloat3(&worldForward), rotationQuaternion));

	vectorsDirty = false;
}
