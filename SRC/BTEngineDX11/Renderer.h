#pragma once

using namespace SimpleMath;

class ModelData;
class ModelDataSet;

class Renderer : public BTBehaviour
{
public:
	Renderer();
	virtual ~Renderer();

public:
	// 새로 선언한 가상
	virtual void Render(Camera* pCamera) abstract;

	// 재정의한 가상
	virtual void Update(float deltaTime) override;

	void UpdateRenderer(Camera* pCamera);
	void SettingBuffers(ModelData* meshData);
	void SetMeshDatasByStr(wstring dataName);

	void CommonSetting();

public:
	ModelDataSet* mModelDataSet;	// 현재 MeshDatas

protected:
	ID3D11DeviceContext* mDeviceContext;
	ID3D11RasterizerState* mpRenderState;	// 렌더 상태

	Effect* mEffect;

	UINT mOffset;
	ID3D11InputLayout* mInputLayout;	// 인풋 레이아웃

	D3D_PRIMITIVE_TOPOLOGY mPrimitiveTopo;

	// 행렬
	Matrix mWorld;		// 월드 변환 행렬
	Matrix mView;		// 시야 변환 행렬
	Matrix mProj;		// 투영 변환 행렬

	Vector3 mEyePos;

};