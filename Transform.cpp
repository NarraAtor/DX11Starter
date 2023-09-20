#include "Transform.h"

using namespace DirectX;
Transform::Transform() :
	position(0, 0, 0),
	scale(1, 1, 1),
	pitchYawRoll(0, 0, 0)
{
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
	matrixDirty = false;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
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
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
}

void Transform::SetScale(float x, float y, float z)
{
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
}
