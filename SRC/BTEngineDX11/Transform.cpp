#include "pch.h"
#include "Transform.h"

Transform::Transform()
{
	Position.x = 0;	Position.y = 0;	Position.z = 0;
	Rotation.x = 0;	Rotation.y = 0;	Rotation.z = 0;
	Scale.x = 1;	Scale.y = 1;	Scale.z = 1;
}

Transform::~Transform()
{
}

// 미리 구해놔도 되나?
Vector3 Transform::GetForward()
{
	float rotY = Rotation.y;

	Matrix rot = Matrix::CreateFromYawPitchRoll(Rotation.y, Rotation.x, Rotation.z);

	Vector3 forward = XMVector3TransformNormal(Vector3(0, 0, 1), rot);

	return forward;
}

Matrix Transform::GetTransformMatrix()
{
	Matrix trans;
	trans.Translation(Position);

	Matrix rot = Matrix::CreateFromYawPitchRoll(Rotation.y, Rotation.x, Rotation.z);

	Matrix scale = Matrix::CreateScale(Scale);

	return scale  * rot * trans;
}
