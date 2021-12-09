#pragma once
class ResourceManager
{
private:
	static ResourceManager* mInstance;
	ResourceManager();

public:
	static ResourceManager* GetInstance();
	~ResourceManager();

private:
	// Key[ 데이터 이름 ], value[ Key[ 노드 이름 ], value[ 지오메트리 ] ]
	unordered_map<wstring, ModelDataSet*> mModelDataSetMap;

	// Key[ 클립 이름 ], value[ 애니메이션모음 ( Animation은 노드1개의 움직을 표현) ]
	unordered_map<wstring, AnimationClip* > mAnimationClipMap;

	// Key[ 재질 명 ], value[ 재질목록 ]
	unordered_map<wstring, vector<BTMaterial*>> mBTMaterialVecMap;

	// Key[ 텍스쳐 이름 ], value[ 리소스 ]
	unordered_map<wstring, ID3D11ShaderResourceView*> mTextureMap;

public:
	void Init();

	void LoadTextureFromDDS	(wstring dataName, wstring pFilePath);
	void LoadTextureFromPng(wstring dataName, wstring pFilePath);
	void LoadModelDataSet (wstring dataName, wstring pFilePath);
	void LoadAnimation	(wstring dataName, wstring pFilePath);

public:

	// Model정보 만들기
	template<typename T>
	ModelData* CreateModelData(MeshData<T>* data);

	// Helper
	ModelData* LoadAxis();
	ModelData* LoadGrid();
	ModelData* LoadSpriteSquare();

	// Basic
	ModelData* LoadBox();
	ModelData* LoadSphere(float radius, UINT sliceCount, UINT stackCount);

	// Sky
	ModelData* LoadSkySphere(float radius, UINT sliceCount, UINT stackCount);

	// Mesh정보 만들기
	MeshData<BTStruct::Vertex>* CreateMeshData(ASEParser::Mesh* data);
	MeshData<BTStruct::VertexSkin>* CreateMeshSkinData(ASEParser::Mesh* data);

	MeshData<BTStruct::Vertex>* GetMeshData_Sphere(float radius, UINT sliceCount, UINT stackCount);

public:
	ModelDataSet*	GetModelDatasOrginal(wstring name);
	ModelDataSet*	GetModelDatas(wstring name);

	AnimationClip*	GetAniClipData(wstring name);

	BTMaterial*		GetMaterial(wstring name, int refNum);

	ID3D11ShaderResourceView* GetTexture(wstring name);
};