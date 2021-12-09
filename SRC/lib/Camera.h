#pragma once

class ModelData;
class ModelDataSet;

class Camera
{
public:
	Camera();
	~Camera();

	void SetLens(float fovY, float aspect, float zn, float zf);

	void LookAt(Vector3 pos, Vector3 target, Vector3 worldUP);

	void Walk	(float deltaTime);
	void Strafe	(float deltaTime);
	void UpDown	(float deltaTime);

	void Pitch(float angle);
	void RotateY(float angle);

	void CameraMove(float deltaTime);
	void CameraRotate(float x, float y, float lastX, float lastY);

	void UpdateViewMatrix();
	void Render();

//Get, Set 함수들
public: 
	Matrix GetView();
	Matrix GetProj();

	void SetProj(Matrix proj);

	Vector3 GetPos();
	Vector3 GetLook();

	float GetPitchAngle();
	float GetRotYAngle();

	void SetCubeMap(wstring name);

public:
	bool IsEditorCamera;

private:
	POINT mLastMousePos;

	Vector3 mPosition; //위치
	Vector3 mRight;	//오른쪽
	Vector3 mUp;		//위
	Vector3 mLook;		//시선

	float mPitchAngle;
	float mRotYAngle;

	// 프러스텀 요소
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;

	// 뷰 스페이스(???)
	float mNearWindowHeight; 
	float mFarWindowHeight;

	Matrix mView;
	Matrix mProj;

	// SkyBox 렌더링
	UINT mOffset;
	ModelDataSet* mSkySphere;

	ID3D11ShaderResourceView* mCubeSRV;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11RasterizerState* mpRenderState;
	ID3D11InputLayout* mInputLayout;
	D3D_PRIMITIVE_TOPOLOGY mPrimitiveTopo;
};

