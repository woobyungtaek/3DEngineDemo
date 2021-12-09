#pragma once
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

class Transform 
{
public:
	Transform();
	~Transform();

public:
	Vector3 Position;
	Vector3 Rotation;
	Vector3 Scale;

	Vector3 GetForward();
	Matrix  GetTransformMatrix();
};

