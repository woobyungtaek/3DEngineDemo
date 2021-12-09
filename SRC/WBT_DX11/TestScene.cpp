#include "pch.h"
#include "TestScene.h"


TestScene::TestScene()
{
	IsStarted = false;
	IsEnable = true;

	mJoe1 = nullptr;
	mJoe2 = nullptr;
	mGenji = nullptr;
}

TestScene::~TestScene()
{
}

void TestScene::Start(float deltaTime)
{
	// 리소스 로드
	BTLoader::LoadModelDataSet(L"Genji", L"../ASE/genji_max.ASE");
	BTLoader::LoadModelDataSet(L"Joe_Mesh", L"../ASE/03IK-Joe.ASE");
	BTLoader::LoadModelDataSet(L"teapot", L"../ASE/teapot.ASE");
	BTLoader::LoadModelDataSet(L"Ant3", L"../ASE/ant3.ase");
	BTLoader::LoadModelDataSet(L"Cylinder", L"../ASE/Cylinder.ASE");

	BTLoader::LoadAnimation(L"Joe_Ani", L"../ASE/03IK-Joe.ASE");
	BTLoader::LoadAnimation(L"Ant3Ani", L"../ASE/ant3.ase");
	BTLoader::LoadAnimation(L"CylinderAni", L"../ASE/Cylinder.ASE");

	BTLoader::LoadTextureFromDDS(L"BoxDDS", L"../Textures/WoodCrate02.dds");
	BTLoader::LoadTextureFromDDS(L"GenjiTex", L"../Textures/000000002405.dds");
	BTLoader::LoadTextureFromDDS(L"GenjiSubTex", L"../Textures/000000002476.dds");
	BTLoader::LoadTextureFromDDS(L"GenjiNormal", L"../Textures/Genji_nmap.dds");
	BTLoader::LoadTextureFromDDS(L"brick", L"../Textures/darkbrickdxt1.dds");
	BTLoader::LoadTextureFromDDS(L"grasscube", L"../Textures/grasscube1024.dds");
	BTLoader::LoadTexturePng(L"testBrickNormal", L"../Textures/testBrickNormal.png");
	BTLoader::LoadTexturePng(L"TileMapImage", L"../Textures/TileMapImage.png");
	BTLoader::LoadTexturePng(L"Character_Hair", L"../Textures/Character_Hair.png");

	//mEditorCamera->SetCubeMap(L"grasscube");

	// 겐지 0번 재질은 ASE파일에 포함되어있다.
	BTMaterial* genjiMat = BTResource::GetMaterial(L"Genji", 0);
	BTResource::SetDiffuseMapOnMat(genjiMat, L"GenjiTex");
	BTResource::SetNormalMapOnMat(genjiMat, L"GenjiNormal");

	BTMaterial* genjiSubMat = new BTMaterial();
	*genjiSubMat->mProperties = *genjiMat->mProperties;
	BTResource::SetDiffuseMapOnMat(genjiSubMat, L"GenjiSubTex");

	BTResource::SetMatOnModelData(L"Genji", 2, genjiSubMat);
	BTResource::SetMatOnModelData(L"Genji", 3, genjiSubMat);

	// 조들 생성하기
	mJoe1 = new GameObject();
	mJoe2 = new GameObject();
	mGenji = new GameObject();
	mBox = new GameObject();
	mGrim2 = new GameObject();
	mGrim = new GameObject();

	// Genji
	MeshRenderer* tempMR3 = mGenji->AddComponent<MeshRenderer>();
	tempMR3->SetMeshDatasByStr(L"Genji");

	mGenji->mTransform->Position.z = 10.0f;
	mGenji->mTransform->Scale = Vector3(5.f, 5.f, 5.f);

	//실린더
	MeshRenderer* tempMR1 = mBox->AddComponent<MeshRenderer>();
	tempMR1->SetMeshDatasByStr(L"Cylinder");

	mBox->mTransform->Scale = Vector3(0.1f, 0.1f, 0.1f);
	mBox->mTransform->Position += Vector3(10,0, 10);

	Animator* tempAnimator0 = mBox->AddComponent<Animator>();
	tempAnimator0->AddAnimationClipByStr(L"CylinderAni");
	tempAnimator0->SetAnimationClipByIdx(0);
	tempAnimator0->Play(true);

	// Joe1
	MeshRenderer* tempMR2 = mJoe1->AddComponent<MeshRenderer>();
	tempMR2->SetMeshDatasByStr(L"Joe_Mesh");

	Animator* tempAnimator1 = mJoe1->AddComponent<Animator>();
	tempAnimator1->AddAnimationClipByStr(L"Joe_Ani");
	tempAnimator1->SetAnimationClipByIdx(0);

	mJoe1->mTransform->Scale = Vector3(0.05f, 0.05f, 0.05f);

	// Joe2
	MeshRenderer* tempMR4 = mJoe2->AddComponent<MeshRenderer>();
	tempMR4->SetMeshDatasByStr(L"Joe_Mesh");

	Animator* tempAnimator2 = mJoe2->AddComponent<Animator>();
	tempAnimator2->AddAnimationClipByStr(L"Joe_Ani");
	tempAnimator2->SetAnimationClipByIdx(0);

	mJoe2->mTransform->Position.x = 10.0f;
	mJoe2->mTransform->Scale = Vector3(0.05f, 0.05f, 0.05f);


	SpriteRenderer* spriteRD2 = mGrim2->AddComponent<SpriteRenderer>();
	spriteRD2->SetSpriteByName(L"TileMapImage");

	SpriteRenderer* spriteRD1 = mGrim->AddComponent<SpriteRenderer>();
	spriteRD1->SetSpriteByName(L"testBrickNormal");
	mGrim->mTransform->Scale = Vector3(0.3f, 0.3f, 0.3f);
	mGrim->mTransform->Position = Vector3(0.3f, 0.3f, 0);

}

void TestScene::Update(float deltaTime)
{
	if (IsStarted == false)
	{
		IsStarted = true;
		Start(deltaTime);
		return;
	}

	if (BTInputEvent::InputKeyDown('J'))
	{
		mJoe1->GetComponent<Animator>()->Play(true);
	}
	if (BTInputEvent::InputKeyDown('N'))
	{
		mJoe1->GetComponent<Animator>()->Pause();
	}
	if (BTInputEvent::InputKeyDown('U'))
	{
		mJoe1->GetComponent<Animator>()->Resume();
	}
	if (BTInputEvent::InputKeyDown('Y'))
	{
		mJoe1->GetComponent<Animator>()->Stop();
	}
	if (BTInputEvent::InputKeyDown('K'))
	{
		mJoe2->GetComponent<Animator>()->PlayWithDelay(3.0f);
	}
	if (BTInputEvent::InputKey('C'))
	{
		mGenji->mTransform->Rotation.x += 10.f * deltaTime;
	}
	if (BTInputEvent::InputKey('V'))
	{
		mGenji->mTransform->Rotation.y += 10.f * deltaTime;
		mJoe1->mTransform->Rotation.y  += 10.f * deltaTime;
	}
	if (BTInputEvent::InputKey('B'))
	{
		mGenji->mTransform->Rotation.z += 10.f * deltaTime;
	}
	if (BTInputEvent::InputKey('T'))
	{
		mGenji->mTransform->Position += mGenji->mTransform->GetForward() * deltaTime;
	}
	if (BTInputEvent::InputKey('R'))
	{
		mGenji->mTransform->Position += -mGenji->mTransform->GetForward() * deltaTime;
	}
}

void TestScene::End(float deltaTime)
{
}
