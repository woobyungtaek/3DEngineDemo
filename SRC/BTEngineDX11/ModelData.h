#pragma once

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

class ModelData
{
public:
	ModelData();
	~ModelData();

public:
	// 공유 자원
	ID3D11Buffer* VB;
	ID3D11Buffer* IB;

	vector<Bone> mBoneList;

	// 비 공유 자원
	bool mbNoBuffer;
	bool mbSkin;

	int mMaterialRef;

	wstring mNodeName;
	wstring mParentNodeName;

	UINT VertexCount;
	UINT FaceCount;
	UINT IndexCount;
	UINT Stride;

	// 부모 노드와 필요 행렬들
	ModelData* mParentMeshData;
	Matrix mNodeMatrix;		// 오염되면 안되는 TM (노드)
	Matrix mLocalMatrix;	// 오염되면 안되는 TM (재구성된 로컬)
	Matrix mWorldMatrix;	// 
	Matrix mUseMatrix;		// 공통으로 사용될 TM

	vector<XMFLOAT4X4> mFinalBoneTMVec;

	// Material Data
	BTMaterial* mMaterial;

public:
	ModelData* GetCopy();

	void SetLocalMatrix();
	void SetAnimationMatrix(Animation* aniData, int tickFrame);
	void SetBoneMatrix();

	Matrix GetInvertWorldMatrix();
	Matrix GetWorldMatrix();
	Matrix GetUseMatrixTM();

	void SetEffectAndInputLayout(Effect** refEffect, ID3D11InputLayout** refInputLayout);
};