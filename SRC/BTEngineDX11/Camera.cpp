#include "pch.h"
#include "Camera.h"

Camera::Camera() :
	mPosition(0.0f,0.0f,0.0f),
	mRight(1.0f,0.0f,0.0f),
	mUp(0.0f,1.0f,0.0f),
	mLook(0.0f,0.0f,1.0f),
	mPitchAngle(0.0f),
	mRotYAngle(0.0f),
	IsEditorCamera(false)
{
	SetLens(0.25f * MathHelper::Pi, 1.0f, 0.0001f,1000.0f);
	D3D11Render* instRender = BTGameEngine::GetInstance()->GetRender();
	mDeviceContext = instRender->GetDeviceContext();
}

Camera::~Camera()
{
}

void Camera::SetLens(float fovY, float aspect, float nearZ, float farZ)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = nearZ;
	mFarZ = farZ;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	Matrix proj = Matrix::CreatePerspectiveFieldOfView(mFovY, mAspect, mNearZ, mFarZ);
	// 3번째 요소들이 반전되어있다...
	proj._31 *= -1;	proj._32 *= -1;	proj._33 *= -1;	proj._34 *= -1;
	mProj = proj;
}

void Camera::LookAt(Vector3 pos, Vector3 target, Vector3 worldUP)
{
	Vector3 look = target - pos;
	look.Normalize();
	Vector3 right = worldUP.Cross(look);
	Vector3 up = look.Cross(right);

	mPosition = pos;
	mLook = look;
	mRight = right;
	mUp = up;
}

void Camera::Walk(float deltaTime)
{
	Vector3 scale	= Vector3(deltaTime, deltaTime, deltaTime); 
	Vector3 look	= mLook;
	Vector3 pos		= mPosition;

	mPosition = (scale * look) + pos;
}

void Camera::Strafe(float deltaTime)
{
	Vector3 scale = Vector3(deltaTime, deltaTime, deltaTime);
	Vector3 right = mRight;
	Vector3 pos = mPosition;

	mPosition = (scale * right) + pos;
}

void Camera::UpDown(float deltaTime)
{
	Vector3 scale = Vector3(deltaTime, deltaTime, deltaTime);
	Vector3 up = mUp;
	Vector3 pos = mPosition;

	mPosition = (scale * up) + pos;
}

void Camera::Pitch(float angle)
{
	mPitchAngle = angle;
	Matrix rot = Matrix::CreateFromAxisAngle(mRight, mPitchAngle);

	mUp		= XMVector3TransformNormal(mUp, rot);
	mLook = XMVector3TransformNormal(mLook, rot);
}

void Camera::RotateY(float angle)
{
	mRotYAngle = angle;
	Matrix rot = Matrix::CreateRotationY(mRotYAngle);

	mRight	= XMVector3TransformNormal(mRight, rot);
	mUp		= XMVector3TransformNormal(mUp, rot);
	mLook	= XMVector3TransformNormal(mLook, rot);
}

void Camera::UpdateViewMatrix()
{
	Vector3 right		= mRight;
	Vector3 up			= mUp;
	Vector3 look		= mLook;
	Vector3 position	= mPosition;

	// 시선과 오른쪽 벡터 외적 = 업 벡터
	
	look.Normalize();
	up = look.Cross(right);

	// 업 벡터와 룩 벡터 외적
	right = up.Cross(look);

	float x = -position.Dot(right);
	float y = -position.Dot(up);
	float z = -position.Dot(look);

	mRight = right;
	mUp = up;
	mLook = look;

	mView(0, 0) = mRight.x; mView(0, 1) = mUp.x; mView(0, 2) = mLook.x; mView(0, 3) = 0.0f;
	mView(1, 0) = mRight.y;	mView(1, 1) = mUp.y; mView(1, 2) = mLook.y;	mView(1, 3) = 0.0f;
	mView(2, 0) = mRight.z;	mView(2, 1) = mUp.z; mView(2, 2) = mLook.z;	mView(2, 3) = 0.0f;
	mView(3, 0) = x;		mView(3, 1) = y;	 mView(3, 2) = z;		mView(3, 3) = 1.0f;
		
}

void Camera::Render()
{
	if (mCubeSRV == nullptr) { return; }
	if (mSkySphere == nullptr)
	{
		mSkySphere = ResourceManager::GetInstance()->GetModelDatas(L"SkySphere");
	}

	Matrix pTM;
	pTM.Translation(mPosition);
	
	Matrix view = GetView();
	Matrix proj = GetProj();

	Matrix wvpTM;
	wvpTM = pTM * view * proj;

	Effects::SkyFX->SetWorldViewProj(wvpTM);
	Effects::SkyFX->SetCubeMap(mCubeSRV); // Texture 

	mOffset = 0;
	UINT stride = mSkySphere->mModelDataVec[0]->Stride;
	mSkySphere->mModelDataVec[0]->VB;

	mDeviceContext->IASetVertexBuffers(0, 1, 
		&mSkySphere->mModelDataVec[0]->VB, 
		&stride,
		&mOffset);
	mDeviceContext->IASetIndexBuffer(mSkySphere->mModelDataVec[0]->IB, DXGI_FORMAT_R32_UINT, 0);
	mDeviceContext->IASetInputLayout(InputLayouts::SkyBox);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::SkyFX->mTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = Effects::SkyFX->mTech->GetPassByIndex(p);

		pass->Apply(0, mDeviceContext);

		mDeviceContext->DrawIndexed(mSkySphere->mModelDataVec[0]->IndexCount, 0, 0);
	}
}

Matrix Camera::GetView()
{
	return mView;
}

Matrix Camera::GetProj()
{
	return mProj;
}

void Camera::SetProj(Matrix proj)
{
	mProj = proj;
}

Vector3 Camera::GetPos()
{
	return mPosition;
}

Vector3 Camera::GetLook()
{
	return mLook;
}

float Camera::GetPitchAngle()
{
	return mPitchAngle;
}

float Camera::GetRotYAngle()
{
	return mRotYAngle;
}

void Camera::SetCubeMap(wstring name)
{
	mCubeSRV = ResourceManager::GetInstance()->GetTexture(name);
}

// Editor 카메라 움직임
void Camera::CameraMove(float deltaTime)
{
	if (BTInput::InputKeyDown(VK_RBUTTON) || BTInput::InputKeyDown(VK_LBUTTON))
	{
		// Mouse Down
		mLastMousePos = BTInput::GetMousePosSC();
	}
	if (BTInput::InputKey(VK_RBUTTON))	
	{
		// Mouse Down Ing..
		// 카메라 회전 함수 실행
		POINT mousePos = BTInput::GetMousePosSC();
		CameraRotate((float)mousePos.x, (float)mousePos.y, (float)mLastMousePos.x, (float)mLastMousePos.y);
		mLastMousePos = mousePos;
	}

	if (BTInput::InputKey('W') )
	{
		Walk(10.0f * deltaTime);
	}
	if (BTInput::InputKey('S') )
	{
		Walk(-10.0f * deltaTime);
	}
	if (BTInput::InputKey('A') )
	{
		Strafe(-10.0f * deltaTime);
	}
	if (BTInput::InputKey('D') )
	{
		Strafe(10.0f * deltaTime);
	}
	if (BTInput::InputKey('Q') )
	{
		UpDown(-10.0f * deltaTime);
	}
	if (BTInput::InputKey('E') )
	{
		UpDown(10.0f * deltaTime);
	}
}

void Camera::CameraRotate(float x, float y, float lastX, float lastY)
{
	float degreeX = XMConvertToRadians(0.25f * (x - lastX));
	float degreeY = XMConvertToRadians(0.25f * (y - lastY));

	Pitch(degreeY);
	RotateY(degreeX);
}
