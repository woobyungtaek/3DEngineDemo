/*
	파일 이름을 입력하면
	Mesh, Animation등의 데이터를 만들어 낸다.
	생성된 데이터는 데이터 매니져에 따로 저장되도록 한다.
*/

#pragma once

enum eObjectType
{
	eObjectType_ObjectRoot, eObjectType_Geomobject, eObjectType_Shape,
	eObjectType_Camera, eObjectType_Light, eObjectType_HelperObject,
	eObjectType_MarkerObject, eObjectType_SkinnedMesh, eObjectType_Bone,
	eObjectType_CMeshPlane, eObjectType_CBullet_Trace,
	//
	eObjectType_BasicSphere, eObjectType_Missile,
	eObjectType_MeshBillboard, eObjectType_PointSprite,			// CMesh상속..
	eObjectType_End,
};

#include "ASEFile.h"

#include "ParsingDataDefine.h"

using namespace ASEParser;

class Animation;

// 애니메이션(노드별)을 묶을 Clip
class AnimationClip
{
public:
	AnimationClip() {}
	~AnimationClip() {}

public:
	// 데이터에 있는 SceneData
	Scenedata mSceneData;

	std::wstring mClipName;

	// Key[ 노드이름 ], Value[ 애니메이션 ]
	unordered_map<std::wstring, Animation*> mAnimationMap;
};

class BTParseData
{
public:
	BTParseData();
	~BTParseData();

public:

	// 3D Max Parsing시 붙어 나오는 값
	int mAsciiExport;

	// Material 리스트
	int mMaterialCount;
	vector< ASEParser::ASEMaterial*> mMaterialDataVec;

	// Mesh 리스트
	vector< ASEParser::Mesh*> mMeshVec;

public:
	void SetParentNode();
};

class BTASEParser
{
private:
	static ASE::CASELexer* mLexer;
	static char mTokenStr[256];

	static 	bool mbAnimation; // 애니메이션 읽는 중
	static  Animation* mAnimationTemp; // 현재 읽는 애니메이션

public:
	static BTParseData* Load(LPSTR pFile);
	static AnimationClip* LoadAni(LPSTR pFile);

private:
	static BTParseData* ParsingAll();
	static AnimationClip* ParsingAnimation();

	static void		Optimize(ASEParser::Mesh* pMesh);
	static void		ConvertNodeTM(ASEParser::Mesh* pMesh);

	static int		Parsing_NumberInt();	// int
	static long		Parsing_NumberLong();	// long을 읽어서 리턴해준다.
	static float	Parsing_NumberFloat();	// float
	static LPSTR	Parsing_String();		// string
	static Vector3	Parsing_NumberVector3();// 3개의 Float를 벡터 하나로 읽어서 리턴해준다
	
};

