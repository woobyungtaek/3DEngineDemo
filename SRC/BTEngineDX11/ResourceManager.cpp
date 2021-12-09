#include "pch.h"
#include "ResourceManager.h"
#define CELL_SIZE 10.0f

ResourceManager* ResourceManager::mInstance;

ResourceManager::ResourceManager()
{
}

ResourceManager* ResourceManager::GetInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new ResourceManager();
	}
	return mInstance;
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Init()
{
	HRESULT hr = CoInitialize(NULL);

	// HelperObject 추가
	mModelDataSetMap.insert(make_pair(L"Axis", new ModelDataSet()));
	mModelDataSetMap[L"Axis"]->AddModelData(L"Axis", LoadAxis());

	mModelDataSetMap.insert(make_pair(L"Grid", new ModelDataSet()));
	mModelDataSetMap[L"Grid"]->AddModelData(L"Grid", LoadGrid());

	mModelDataSetMap.insert(make_pair(L"Sprite2D", new ModelDataSet()));
	mModelDataSetMap[L"Sprite2D"]->AddModelData(L"Sprite2D", LoadSpriteSquare());

	// BasicObject 추가
	// Box
	mModelDataSetMap.insert(make_pair(L"Box", new ModelDataSet()));
	mModelDataSetMap[L"Box"]->AddModelData(L"Box", LoadBox());

	// Sphere
	mModelDataSetMap.insert(make_pair(L"Sphere", new ModelDataSet()));
	mModelDataSetMap[L"Sphere"]->AddModelData(L"Sphere", LoadSphere(0.5, 15, 15));

	// Sphere
	mModelDataSetMap.insert(make_pair(L"SkySphere", new ModelDataSet()));
	mModelDataSetMap[L"SkySphere"]->AddModelData(L"SkySphere", LoadSkySphere(5000, 15, 15));

	// 기본 Material 추가
	BTMaterial* basicMate = new BTMaterial();
	basicMate->mProperties->Ambient = Vector4(0.2f, 0.0f, 0.2f, 1.0f);// 주변
	basicMate->mProperties->Diffuse = Vector4(1.f, 1.f, 1.f, 1.0f); // 확산
	basicMate->mProperties->Specular = Vector4(0.0f, 0.0f, 0.0f, 1.0f);// 반사
	basicMate->mProperties->Reflect = Vector4(0, 0, 0, 0);
	mBTMaterialVecMap.insert(make_pair(L"BasicMat", vector<BTMaterial*>()));
	mBTMaterialVecMap[L"BasicMat"].push_back(basicMate);
}

// 파일로부터 텍스쳐 추가
void ResourceManager::LoadTextureFromDDS(wstring dataName, wstring pFilePath)
{
	ID3D11Device* device = BTGameEngine::GetInstance()->GetRender()->GetDevice();

	ID3D11ShaderResourceView* diffuseMapSRV;

	ID3D11Resource* texResource = nullptr;

	HR(CreateDDSTextureFromFile(device, pFilePath.c_str(), &texResource, &diffuseMapSRV));
	ReleaseCOM(texResource)

	mTextureMap.insert(make_pair(dataName, diffuseMapSRV));
}

// 파일로 부터 
void ResourceManager::LoadTextureFromPng(wstring dataName, wstring pFilePath)
{
	ID3D11Device* device = BTGameEngine::GetInstance()->GetRender()->GetDevice();

	ID3D11ShaderResourceView* diffuseMapSRV;

	ID3D11Resource* texResource = nullptr;

	HR(CreateWICTextureFromFile(device, pFilePath.c_str(), &texResource, &diffuseMapSRV));
	ReleaseCOM(texResource)

		mTextureMap.insert(make_pair(dataName, diffuseMapSRV));
}

// 파일로부터 MeshData 추가
void ResourceManager::LoadModelDataSet(wstring dataName, wstring pFilePath)
{
	string temp;
	temp.assign(pFilePath.begin(), pFilePath.end());
	BTParseData* instData = BTASEParser::Load((LPSTR)temp.c_str());

	if (instData == nullptr)
	{
		TRACE("파일을 여는 중에 문제가 발생했습니다!");
		return;
	}

	// MaterialProperties 추가
	BTMaterial* tempMat = new BTMaterial();
	ASEMaterial* tempASEMAT = new ASEMaterial();
	for (int index = 0; index < (int)instData->mMaterialDataVec.size(); ++index)
	{
		tempMat = new BTMaterial();
		tempASEMAT = instData->mMaterialDataVec[index];

		tempMat->mMatNum = tempASEMAT->m_materialnumber;

		// 텍스쳐 파일 이름 넣기
		if (tempASEMAT->m_map_diffuse != nullptr && tempASEMAT->m_map_diffuse->m_bitmap.empty() == false)
		{
			string diffuseName = tempASEMAT->m_map_diffuse->m_bitmap;
			int lastIdx = (int)diffuseName.find_last_of('\\') + 1;
			tempMat->mDiffuseName = wstring(diffuseName.begin() + lastIdx, diffuseName.end());
		}
		// 노멀 파일 이름 넣기
		if (tempASEMAT->m_map_generic != nullptr && tempASEMAT->m_map_generic->m_bitmap.empty() == false)
		{
			string normalName = tempASEMAT->m_map_generic->m_bitmap;
			int lastIdx = (int)normalName.find_last_of('\\') + 1;
			tempMat->mNormalMapName = wstring(normalName.begin() + lastIdx, normalName.end());
		}

		// 프로퍼티 넣기
		Vector3 tempVec3;
		tempVec3 = tempASEMAT->m_material_ambient;
		tempMat->mProperties->Ambient = Vector4(tempVec3.x, tempVec3.y, tempVec3.z, 1);

		tempVec3 = tempASEMAT->m_material_diffuse;
		tempMat->mProperties->Diffuse = Vector4(tempVec3.x, tempVec3.y, tempVec3.z, 1);

		tempVec3 = tempASEMAT->m_material_specular;
		tempMat->mProperties->Specular = Vector4(tempVec3.x, tempVec3.y, tempVec3.z, 1);

		tempMat->mProperties->Reflect = Vector4(
			tempASEMAT->m_material_shine,
			tempASEMAT->m_material_shinestrength,
			tempASEMAT->m_material_transparency,
			tempASEMAT->m_material_wiresize);

		if (mBTMaterialVecMap.find(dataName) == mBTMaterialVecMap.end())
		{
			mBTMaterialVecMap.insert(make_pair(dataName, vector<BTMaterial*>()));
		}
		mBTMaterialVecMap[dataName].push_back(tempMat);
	}

	// MeshDatas 추가
	ModelDataSet* modelDataSet = new ModelDataSet();
	mModelDataSetMap.insert(make_pair(dataName, modelDataSet));

	int meshCount = (int)instData->mMeshVec.size();
	for (int index = 0; index < meshCount; ++index)
	{
		// 메쉬 추가
		wstring* nodeName = new wstring(instData->mMeshVec[index]->m_nodename.begin(), instData->mMeshVec[index]->m_nodename.end());
		wstring* parentName = new wstring(instData->mMeshVec[index]->m_nodeparent.begin(), instData->mMeshVec[index]->m_nodeparent.end());

		// GeometryData Load ( 디바이스에 Create Buffer / LoadGeometry)		
		ModelData* instModelData;
		if (instData->mMeshVec[index]->m_is_skinningobject)
		{
			MeshData<BTStruct::VertexSkin>* meshData = CreateMeshSkinData(instData->mMeshVec[index]);
			instModelData = CreateModelData<BTStruct::VertexSkin>(meshData);
			delete meshData;
		}
		else
		{
			MeshData<BTStruct::Vertex>* meshData = CreateMeshData(instData->mMeshVec[index]);
			instModelData = CreateModelData<BTStruct::Vertex>(meshData);
			delete meshData;
		}

		if (instModelData != nullptr)
		{
			// 값 설정 설정
			instModelData->mbSkin = instData->mMeshVec[index]->m_is_skinningobject;
			instModelData->mNodeMatrix = instData->mMeshVec[index]->m_WorldTM;// 노드 TM이 따로 있어야함
			instModelData->mWorldMatrix = instData->mMeshVec[index]->m_WorldTM;
			instModelData->mNodeName = *nodeName;
			instModelData->mParentNodeName = *parentName;
			instModelData->mMaterialRef = instData->mMeshVec[index]->m_material_ref;

			instModelData->mMaterial = nullptr;
			if (mBTMaterialVecMap.find(dataName) != mBTMaterialVecMap.end())
			{
				instModelData->mMaterial = mBTMaterialVecMap[dataName][instModelData->mMaterialRef];
			}
			modelDataSet->AddModelData(*nodeName, instModelData);
		}

		// Mesh로 부터 바로 가져오는 값.
		// BoneList
		int boneCount = instData->mMeshVec[index]->m_vector_bone_list.size();
		if (boneCount > 0 && instModelData != nullptr)
		{
			for (int boneIdx = 0; boneIdx < boneCount; ++boneIdx)
			{
				instModelData->mBoneList.push_back(*instData->mMeshVec[index]->m_vector_bone_list[boneIdx]);
			}
		}

		delete nodeName;
		delete parentName;
	}

	// MeshData의 노드 구성
	modelDataSet->SetParentNode();

	// MeshData의 로컬 매트릭스 생성
	modelDataSet->UpdateLocalMatrix();

	delete instData;
}

// 파일로부터 AnimationData 추가
void ResourceManager::LoadAnimation(wstring dataName, wstring pFilePath)
{
	// 적재 확인
	if (mAnimationClipMap.find(dataName) != mAnimationClipMap.end()) { return; }

	// Animation Data 읽기
	string temp;
	temp.assign(pFilePath.begin(), pFilePath.end());
	AnimationClip* instData = BTASEParser::LoadAni((LPSTR)temp.c_str());

	if (instData == nullptr)
	{
		TRACE("파일을 여는 중에 문제가 발생했습니다!");
		return;
	}

	mAnimationClipMap.insert(make_pair(dataName, instData));
}

#pragma region MeshData 생성


// Skinning은 얘 부터 다르게 불러와야함..
// Vertex에 Weight BoneIndices 붙은 애가 필요
// ASE 파일 Mesh  로드
MeshData<BTStruct::Vertex>* ResourceManager::CreateMeshData(ASEParser::Mesh* data)
{
	MeshData<BTStruct::Vertex>* meshData = new MeshData<BTStruct::Vertex>();

	meshData->mVertexCount = (int)data->m_opt_vertex.size();
	meshData->mFaceCount = (int)data->m_meshface.size();
	meshData->mIndexCount = 3 * meshData->mFaceCount;
	meshData->mStride = sizeof(BTStruct::Vertex);
	meshData->mbSkin = data->m_is_skinningobject;
	meshData->mMaterialRef = data->m_material_ref;

	// 버택스 옮기기
	for (int index = 0; index < meshData->mVertexCount; ++index)
	{
		meshData->mVertices.push_back(BTStruct::Vertex());
		meshData->mVertices[index].Pos = data->m_opt_vertex[index]->m_pos;
		meshData->mVertices[index].Normal = data->m_opt_vertex[index]->m_normal;
		meshData->mVertices[index].UV.x = data->m_opt_vertex[index]->u;
		meshData->mVertices[index].UV.y = 1 - data->m_opt_vertex[index]->v;
	}
	// 탄젠트 설정
	for (int index = 0; index < meshData->mFaceCount; ++index)
	{
		Face* nowFace = data->m_meshface[index];

		// 각 폴리곤을 이루는 버텍스에 접근한다.
		int _vertexIndex01 = nowFace->m_vertexindex[0];
		int _vertexIndex02 = nowFace->m_vertexindex[1];
		int _vertexIndex03 = nowFace->m_vertexindex[2];

		// 노말을 가져온다.
		Vector3 _P0 = data->m_opt_vertex[_vertexIndex01]->m_pos;
		Vector3 _P1 = data->m_opt_vertex[_vertexIndex02]->m_pos;
		Vector3 _P2 = data->m_opt_vertex[_vertexIndex03]->m_pos;

		Vector2 _W0 = Vector2(data->m_opt_vertex[_vertexIndex01]->u, data->m_opt_vertex[_vertexIndex01]->v);
		Vector2 _W1 = Vector2(data->m_opt_vertex[_vertexIndex02]->u, data->m_opt_vertex[_vertexIndex02]->v);
		Vector2 _W2 = Vector2(data->m_opt_vertex[_vertexIndex03]->u, data->m_opt_vertex[_vertexIndex03]->v);

		// 노말로부터 삼각형을 만든다.
		Vector3 _E1 = _P1 - _P0;
		Vector3 _E2 = _P2 - _P0;

		float _X1 = _W1.x - _W0.x;
		float _X2 = _W2.x - _W0.x;
		float _Y1 = _W1.y - _W0.y;
		float _Y2 = _W2.y - _W0.y;

		// 역함수를 구해서 TBN을 구한다. 정확히는 T만 있으면 됨
		float _R = 1.0f / (_X1 * _Y2 - _X2 * _Y1);
		Vector3 _T = (_E1 * _Y2 - _E2 * _Y1) * _R;

		Vector3 _result;
		_T.Normalize(_result);

		meshData->mVertices[_vertexIndex01].Tangent = meshData->mVertices[_vertexIndex01].Tangent + _result;
		meshData->mVertices[_vertexIndex02].Tangent = meshData->mVertices[_vertexIndex02].Tangent + _result;
		meshData->mVertices[_vertexIndex03].Tangent = meshData->mVertices[_vertexIndex03].Tangent + _result;
	}

	// 인덱스 만들기
	for (int index = 0; index < meshData->mFaceCount; ++index)
	{
		meshData->mIndices.push_back(data->m_opt_index[index].index[0]);
		meshData->mIndices.push_back(data->m_opt_index[index].index[2]);
		meshData->mIndices.push_back(data->m_opt_index[index].index[1]);
	}

	return meshData;
}

MeshData<BTStruct::VertexSkin>* ResourceManager::CreateMeshSkinData(ASEParser::Mesh* data)
{
	MeshData<BTStruct::VertexSkin>* meshData = new MeshData<BTStruct::VertexSkin>();

	meshData->mVertexCount = (int)data->m_opt_vertex.size();
	meshData->mFaceCount = (int)data->m_meshface.size();
	meshData->mIndexCount = 3 * meshData->mFaceCount;
	meshData->mStride = sizeof(BTStruct::VertexSkin);
	meshData->mbSkin = data->m_is_skinningobject;
	meshData->mMaterialRef = data->m_material_ref;

	// 버택스 옮기기
	for (int index = 0; index < meshData->mVertexCount; ++index)
	{
		meshData->mVertices.push_back(BTStruct::VertexSkin());
		meshData->mVertices[index].Pos = data->m_opt_vertex[index]->m_pos;
		meshData->mVertices[index].Normal = data->m_opt_vertex[index]->m_normal;
		meshData->mVertices[index].UV.x = data->m_opt_vertex[index]->u;
		meshData->mVertices[index].UV.y = 1 - data->m_opt_vertex[index]->v;

		for (int wIdx = 0; wIdx < data->m_opt_vertex[index]->m_bone_blending_weight.size(); wIdx++)
		{
			switch (wIdx)
			{
			case 0:
				meshData->mVertices[index].Weights.x = data->m_opt_vertex[index]->m_bone_blending_weight[wIdx]->m_bone_weight;
				meshData->mVertices[index].BoneIndices[wIdx] = data->m_opt_vertex[index]->m_bone_blending_weight[wIdx]->m_bone_number;
				break;
			case 1:
				meshData->mVertices[index].Weights.y = data->m_opt_vertex[index]->m_bone_blending_weight[wIdx]->m_bone_weight;
				meshData->mVertices[index].BoneIndices[wIdx] = data->m_opt_vertex[index]->m_bone_blending_weight[wIdx]->m_bone_number;
				break;
			case 2:
				meshData->mVertices[index].Weights.z = data->m_opt_vertex[index]->m_bone_blending_weight[wIdx]->m_bone_weight;
				meshData->mVertices[index].BoneIndices[wIdx] = data->m_opt_vertex[index]->m_bone_blending_weight[wIdx]->m_bone_number;
				break;
			}
		}
	}

	//// 탄젠트 설정
	//for (int index = 0; index < meshData->mFaceCount; ++index)
	//{
	//	Face* nowFace = data->m_meshface[index];

	//	// 각 폴리곤을 이루는 버텍스에 접근한다.
	//	int _vertexIndex01 = nowFace->m_vertexindex[0];
	//	int _vertexIndex02 = nowFace->m_vertexindex[1];
	//	int _vertexIndex03 = nowFace->m_vertexindex[2];

	//	// 노말을 가져온다.
	//	Vector3 _P0 = data->m_opt_vertex[_vertexIndex01]->m_pos;
	//	Vector3 _P1 = data->m_opt_vertex[_vertexIndex02]->m_pos;
	//	Vector3 _P2 = data->m_opt_vertex[_vertexIndex03]->m_pos;

	//	Vector2 _W0 = Vector2(data->m_opt_vertex[_vertexIndex01]->u, data->m_opt_vertex[_vertexIndex01]->v);
	//	Vector2 _W1 = Vector2(data->m_opt_vertex[_vertexIndex02]->u, data->m_opt_vertex[_vertexIndex02]->v);
	//	Vector2 _W2 = Vector2(data->m_opt_vertex[_vertexIndex03]->u, data->m_opt_vertex[_vertexIndex03]->v);

	//	// 노말로부터 삼각형을 만든다.
	//	Vector3 _E1 = _P1 - _P0;
	//	Vector3 _E2 = _P2 - _P0;

	//	float _X1 = _W1.x - _W0.x;
	//	float _X2 = _W2.x - _W0.x;
	//	float _Y1 = _W1.y - _W0.y;
	//	float _Y2 = _W2.y - _W0.y;

	//	// 역함수를 구해서 TBN을 구한다. 정확히는 T만 있으면 됨
	//	float _R = 1.0f / (_X1 * _Y2 - _X2 * _Y1);
	//	Vector3 _T = (_E1 * _Y2 - _E2 * _Y1) * _R;

	//	Vector3 _result;
	//	_T.Normalize(_result);

	//	meshData->mVertices[_vertexIndex01].Tangent = meshData->mVertices[_vertexIndex01].Tangent + _result;
	//	meshData->mVertices[_vertexIndex02].Tangent = meshData->mVertices[_vertexIndex02].Tangent + _result;
	//	meshData->mVertices[_vertexIndex03].Tangent = meshData->mVertices[_vertexIndex03].Tangent + _result;
	//}

	// 인덱스 만들기
	for (int index = 0; index < meshData->mFaceCount; ++index)
	{
		meshData->mIndices.push_back(data->m_opt_index[index].index[0]);
		meshData->mIndices.push_back(data->m_opt_index[index].index[2]);
		meshData->mIndices.push_back(data->m_opt_index[index].index[1]);
	}

	return meshData;
}

// Sphere Mesh 로드
MeshData<BTStruct::Vertex>* ResourceManager::GetMeshData_Sphere(float radius, UINT sliceCount, UINT stackCount)
{
	MeshData<BTStruct::Vertex>* meshData = new MeshData<BTStruct::Vertex>();

	BTStruct::Vertex topVtx;
	topVtx.Pos = Vector3(0, radius, 0);
	topVtx.Normal = Vector3(0, 1, 0);
	topVtx.UV = Vector2(0, 0);
	topVtx.Tangent = Vector3(1, 0, 0);

	BTStruct::Vertex bottomVtx;
	bottomVtx.Pos = Vector3(0, -radius, 0);
	bottomVtx.Normal = Vector3(0, -1, 0);
	bottomVtx.UV = Vector2(0, 1);
	bottomVtx.Tangent = Vector3(1, 0, 0);

	meshData->mVertices.push_back(topVtx);

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f * XM_PI / sliceCount;

	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			BTStruct::Vertex v;

			// spherical to cartesian
			v.Pos.x = radius * sinf(phi) * cosf(theta);
			v.Pos.y = radius * cosf(phi);
			v.Pos.z = radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			v.Tangent.x = -radius * sinf(phi) * sinf(theta);
			v.Tangent.y = 0.0f;
			v.Tangent.z = +radius * sinf(phi) * cosf(theta);

			Vector3 tan = v.Tangent;
			tan.Normalize();
			v.Tangent = tan;

			Vector3 pos = v.Pos;
			pos.Normalize();
			v.Pos = pos;

			v.UV.x = theta / XM_2PI;
			v.UV.y = phi / XM_PI;

			meshData->mVertices.push_back(v);
		}
	}
	meshData->mVertices.push_back(bottomVtx);


	for (UINT i = 1; i <= sliceCount; ++i)
	{
		meshData->mIndices.push_back(0);
		meshData->mIndices.push_back(i + 1);
		meshData->mIndices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			meshData->mIndices.push_back(baseIndex + i * ringVertexCount + j);
			meshData->mIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData->mIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			meshData->mIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			meshData->mIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData->mIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	UINT southPoleIndex = (UINT)meshData->mVertices.size() - 1;

	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData->mIndices.push_back(southPoleIndex);
		meshData->mIndices.push_back(baseIndex + i);
		meshData->mIndices.push_back(baseIndex + i + 1);
	}

	meshData->mVertexCount = meshData->mVertices.size();
	meshData->mIndexCount = meshData->mIndices.size();
	meshData->mStride = sizeof(BTStruct::Vertex);

	return meshData;
}

#pragma endregion

#pragma region VB & IB 생성

template<typename T>
ModelData* ResourceManager::CreateModelData(MeshData<T>* data)
{
	ID3D11Device* device = BTGameEngine::GetInstance()->GetRender()->GetDevice();

	// Info Setting
	ModelData* resultData = new ModelData();
	resultData->VertexCount = data->mVertexCount;
	resultData->FaceCount = data->mFaceCount;
	resultData->IndexCount = data->mIndexCount;
	resultData->Stride = data->mStride;

	if (resultData->VertexCount <= 0)
	{
		resultData->mbNoBuffer = true;
		return resultData;
	}

	// VertexBuffer 생성
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = data->GetByteWidth_VB();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &data->mVertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &resultData->VB));

	// IndexBuffer 생성
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = data->GetByteWidth_IB();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &data->mIndices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &resultData->IB));

	return resultData;
}

// Axis 로드 ( 헬퍼 오브젝트 )
ModelData* ResourceManager::LoadAxis()
{
	ID3D11Device* device = BTGameEngine::GetInstance()->GetRender()->GetDevice();

	ModelData* resultData = new ModelData();
	resultData->Stride = sizeof(BTStruct::VertexColor);

	BTStruct::VertexColor vertices[] =
	{
		{ Vector3(0.0f, 0.0f, 0.0f),  Vector4((const float*)&Colors::Red)  },	// x축 (빨강)
		{ Vector3(10.0f, 0.0f, 0.0f), Vector4((const float*)&Colors::Red)  },

		{ Vector3(0.0f, 0.0f, 0.0f),  Vector4((const float*)&Colors::Green)},	// y축 (초록)
		{ Vector3(0.0f, 10.0f, 0.0f), Vector4((const float*)&Colors::Green)},

		{ Vector3(0.0f, 0.0f, 0.0f),  Vector4((const float*)&Colors::Blue) },	// z축 (파랑)
		{ Vector3(0.0f, 0.0f, 10.0f), Vector4((const float*)&Colors::Blue) }
	};

	UINT indices[] =
	{
		// x축
		0, 1,
		// y축
		2, 3,
		// z축
		4, 5,
	};
	resultData->IndexCount = 6;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = resultData->Stride * resultData->IndexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &vinitData, &resultData->VB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * resultData->IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &iinitData, &resultData->IB));

	return resultData;
}

// Grid 로드 ( 헬퍼 오브젝트 )
ModelData* ResourceManager::LoadGrid()
{
	ID3D11Device* device = BTGameEngine::GetInstance()->GetRender()->GetDevice();

	ModelData* resultData = new ModelData();
	resultData->Stride = sizeof(BTStruct::VertexColor);

	BTStruct::VertexColor vertices[404];
	for (int index = 0; index < 101; ++index)
	{
		vertices[index].Pos = Vector3((float)(-50 * CELL_SIZE), 0.0f, (float)(index - 50) * CELL_SIZE);
		vertices[index + 101].Pos = Vector3((float)(50 * CELL_SIZE), 0.0f, (float)(index - 50) * CELL_SIZE);

		if (index % 10 == 0)
		{
			vertices[index].Color = Vector4(1.f, 1.f, 1.f, 1.0f);
			vertices[index + 101].Color = Vector4(1.f, 1.f, 1.f, 1.0f);
		}
		else
		{
			vertices[index].Color = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
			vertices[index + 101].Color = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
		}
	}

	for (int index = 0; index < 101; ++index)
	{
		vertices[index + 202].Pos = Vector3((float)(index - 50) * CELL_SIZE, 0.0f, (float)(-50 * CELL_SIZE));
		vertices[index + 101 + 202].Pos = Vector3((float)(index - 50) * CELL_SIZE, 0.0f, (float)(50 * CELL_SIZE));

		if (index % 10 == 0)
		{
			vertices[index + 202].Color = Vector4(1.f, 1.f, 1.f, 1.0f);
			vertices[index + 101 + 202].Color = Vector4(1.f, 1.f, 1.f, 1.0f);
		}
		else
		{
			vertices[index + 202].Color = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
			vertices[index + 101 + 202].Color = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
		}
	}

	UINT indices[404];
	for (int index = 0; index < 101; ++index)
	{
		indices[index * 2] = index;
		indices[index * 2 + 1] = index + 101;
	}

	for (int index = 0; index < 101; ++index)
	{
		indices[index * 2 + 202] = index + 202;
		indices[index * 2 + 1 + 202] = index + 101 + 202;
	}
	resultData->IndexCount = 404;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = resultData->Stride * resultData->IndexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &vinitData, &resultData->VB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * resultData->IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &iinitData, &resultData->IB));

	return resultData;
}

// SpriteSquare 로드
ModelData* ResourceManager::LoadSpriteSquare()
{
	ID3D11Device* device = BTGameEngine::GetInstance()->GetRender()->GetDevice();

	ModelData* resultData = new ModelData();
	resultData->Stride = sizeof(BTStruct::Vertex2D);

	BTStruct::Vertex2D vertices[4] =
	{
		//Square
		{Vector3(-0.5f, 0.5f,0),Vector2(0.0f,0.0f) },//Left Top		//0
		{Vector3(-0.5f,-0.5f,0),Vector2(0.0f,1.0f) },//Left Bottom  //1
		{Vector3( 0.5f, 0.5f,0),Vector2(1.0f,0.0f) },//Right Top	//2
		{Vector3( 0.5f,-0.5f,0),Vector2(1.0f,1.0f) } //Tight Bottom //3
	};

	UINT indices[6] =
	{
		// front face
		2, 1, 0,
		2, 3, 1
	};
	resultData->IndexCount = 6;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = resultData->Stride * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &vinitData, &resultData->VB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &iinitData, &resultData->IB));

	return resultData;
}

// Box 오브젝트 MeshData 생성 만들어야함
// Box 로드 ( 기본 오브젝트 )
ModelData* ResourceManager::LoadBox()
{
	ID3D11Device* device = BTGameEngine::GetInstance()->GetRender()->GetDevice();

	ModelData* resultData = new ModelData();
	resultData->Stride = sizeof(BTStruct::Vertex);

	BTStruct::Vertex vertices[24] =
	{
		//Box
		// Front
		{Vector3(-0.5f,-0.5f,-0.5f),	Vector3(0.0f,0.0f,-1.0f),Vector2(0.0f,1.0f),Vector3(1.0f, 0.0f, 0.0f) },//0
		{Vector3(-0.5f, 0.5f,-0.5f),	Vector3(0.0f,0.0f,-1.0f),Vector2(0.0f,0.0f),Vector3(1.0f, 0.0f, 0.0f) },//1
		{Vector3(0.5f, 0.5f,-0.5f),		Vector3(0.0f,0.0f,-1.0f),Vector2(1.0f,0.0f),Vector3(1.0f, 0.0f, 0.0f) },//2
		{Vector3(0.5f,-0.5f,-0.5f),		Vector3(0.0f,0.0f,-1.0f),Vector2(1.0f,1.0f),Vector3(1.0f, 0.0f, 0.0f) },//3
		// Back
		{Vector3(-0.5f,-0.5f, 0.5f),	Vector3(0.0f,0.0f,1.0f),Vector2(1.0f,1.0f),Vector3(-1.0f, 0.0f, 0.0f)}, //4
		{Vector3(-0.5f, 0.5f, 0.5f),	Vector3(0.0f,0.0f,1.0f),Vector2(1.0f,0.0f),Vector3(-1.0f, 0.0f, 0.0f)}, //5
		{Vector3(0.5f, 0.5f, 0.5f),		Vector3(0.0f,0.0f,1.0f),Vector2(0.0f,0.0f),Vector3(-1.0f, 0.0f, 0.0f)}, //6
		{Vector3(0.5f,-0.5f, 0.5f),		Vector3(0.0f,0.0f,1.0f),Vector2(0.0f,1.0f),Vector3(-1.0f, 0.0f, 0.0f)}, //7*
		// Top
		{Vector3(-0.5f, 0.5f,-0.5f),	Vector3(0.0f,1.0f,0.0f),Vector2(0.0f,1.0f),Vector3(1.0f, 0.0f, 0.0f)}, //8
		{Vector3(-0.5f, 0.5f, 0.5f),	Vector3(0.0f,1.0f,0.0f),Vector2(0.0f,0.0f),Vector3(1.0f, 0.0f, 0.0f)}, //9
		{Vector3(0.5f,  0.5f, 0.5f),	Vector3(0.0f,1.0f,0.0f),Vector2(1.0f,0.0f),Vector3(1.0f, 0.0f, 0.0f)}, //10
		{Vector3(0.5f,  0.5f,-0.5f),	Vector3(0.0f,1.0f,0.0f),Vector2(1.0f,1.0f),Vector3(1.0f, 0.0f, 0.0f)}, //11*
		// Bottom
		{Vector3(-0.5f, -0.5f,-0.5f),	Vector3(0.0f,-1.0f,0.0f),Vector2(1.0f,1.0f) ,Vector3(-1.0f, 0.0f, 0.0f) }, //12
		{Vector3(-0.5f, -0.5f, 0.5f),	Vector3(0.0f,-1.0f,0.0f),Vector2(1.0f,0.0f) ,Vector3(-1.0f, 0.0f, 0.0f) }, //13
		{Vector3(0.5f,  -0.5f, 0.5f),	Vector3(0.0f,-1.0f,0.0f),Vector2(0.0f,0.0f) ,Vector3(-1.0f, 0.0f, 0.0f) }, //14
		{Vector3(0.5f,  -0.5f,-0.5f),	Vector3(0.0f,-1.0f,0.0f),Vector2(0.0f,1.0f) ,Vector3(-1.0f, 0.0f, 0.0f) }, //15*
		// Left
		{Vector3(-0.5f,-0.5f,-0.5f),	Vector3(-1.0f,0.0f,0.0f),Vector2(1.0f,1.0f),Vector3(0.0f, 0.0f, -1.0f)  }, //16
		{Vector3(-0.5f, 0.5f,-0.5f),	Vector3(-1.0f,0.0f,0.0f),Vector2(1.0f,0.0f),Vector3(0.0f, 0.0f, -1.0f)  }, //17
		{Vector3(-0.5f, 0.5f, 0.5f),	Vector3(-1.0f,0.0f,0.0f),Vector2(0.0f,0.0f),Vector3(0.0f, 0.0f, -1.0f)  }, //18
		{Vector3(-0.5f,-0.5f, 0.5f),	Vector3(-1.0f,0.0f,0.0f),Vector2(0.0f,1.0f),Vector3(0.0f, 0.0f, -1.0f)  }, //19*
		// Right									  
		{Vector3(0.5f,-0.5f,-0.5f),		Vector3(1.0f,0.0f,0.0f),Vector2(0.0f,1.0f),Vector3(0.0f, 0.0f, 1.0f)}, //20
		{Vector3(0.5f, 0.5f,-0.5f),		Vector3(1.0f,0.0f,0.0f),Vector2(0.0f,0.0f),Vector3(0.0f, 0.0f, 1.0f)}, //21
		{Vector3(0.5f, 0.5f, 0.5f),		Vector3(1.0f,0.0f,0.0f),Vector2(1.0f,0.0f),Vector3(0.0f, 0.0f, 1.0f)}, //22
		{Vector3(0.5f,-0.5f, 0.5f),		Vector3(1.0f,0.0f,0.0f),Vector2(1.0f,1.0f),Vector3(0.0f, 0.0f, 1.0f)}  //23*
	};

	UINT indices[36] =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// top face
		8, 9, 10,
		8, 10, 11,

		// bottom face
		12, 14, 13,
		12, 15, 14,

		// left face
		16, 18, 17,
		16, 19, 18,

		// right face
		20,21,22,
		20,22,23
	};
	resultData->IndexCount = 36;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = resultData->Stride * 24;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &vinitData, &resultData->VB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &iinitData, &resultData->IB));

	return resultData;
}

// Sphere 로드 ( 기본 오브젝트 )
ModelData* ResourceManager::LoadSphere(float radius, UINT sliceCount, UINT stackCount)
{
	MeshData<BTStruct::Vertex>* sphereMeshData = GetMeshData_Sphere(radius, sliceCount, stackCount);
	ModelData* resultData = CreateModelData<BTStruct::Vertex>(sphereMeshData);

	delete sphereMeshData;

	return resultData;
}

// SkySphere 로드 
ModelData* ResourceManager::LoadSkySphere(float radius, UINT sliceCount, UINT stackCount)
{
	MeshData<BTStruct::Vertex>* sphereMeshData = GetMeshData_Sphere(radius, sliceCount, stackCount);

	// Position으로 교체해야한다.
	MeshData<BTStruct::VertexPos>* skyMeshData = new MeshData<BTStruct::VertexPos>();
	skyMeshData->mVertexCount = sphereMeshData->mVertexCount;
	skyMeshData->mIndexCount = sphereMeshData->mIndexCount;
	skyMeshData->mStride = sizeof(VertexPos);

	skyMeshData->mVertices.resize(skyMeshData->mVertexCount);
	for (int index = 0; index < skyMeshData->mVertexCount; ++index)
	{
		skyMeshData->mVertices[index].Pos = sphereMeshData->mVertices[index].Pos;
	}

	skyMeshData->mIndices.resize(skyMeshData->mIndexCount);
	for (int index = 0; index < skyMeshData->mIndexCount; ++index)
	{
		skyMeshData->mIndices[index] = sphereMeshData->mIndices[index];
	}

	ModelData* resultData = CreateModelData<BTStruct::VertexPos>(skyMeshData);

	delete sphereMeshData;
	delete skyMeshData;

	return resultData;
}

#pragma endregion

#pragma region  Get & Set

ModelDataSet* ResourceManager::GetModelDatasOrginal(wstring name)
{
	if (mModelDataSetMap.find(name) == mModelDataSetMap.end())
	{
		return nullptr;
	}
	return mModelDataSetMap[name];
}

ModelDataSet* ResourceManager::GetModelDatas(wstring name)
{
	if (mModelDataSetMap.find(name) == mModelDataSetMap.end())
	{
		return nullptr;
	}
	return mModelDataSetMap[name]->GetCopy();
}

AnimationClip* ResourceManager::GetAniClipData(wstring name)
{
	return mAnimationClipMap[name];
}

BTMaterial* ResourceManager::GetMaterial(wstring name, int refNum)
{
	if (mBTMaterialVecMap.find(name) != mBTMaterialVecMap.end())
	{
		if (refNum < (int)mBTMaterialVecMap[name].size())
		{
			return mBTMaterialVecMap[name][refNum];
		}
	}
	return nullptr;
}

ID3D11ShaderResourceView* ResourceManager::GetTexture(wstring name)
{
	return mTextureMap[name];
}

#pragma endregion