
#include "pch.h"
#include "EngineScene.h"

EngineScene::EngineScene(IScene* scene)
{
	mScene = scene;
	Object::Scene = this;

	// 폰트
	if (mpFont == nullptr)
	{
		mpFont = new DXTKFont();
	}

	// 에디터 카메라
	mEditorCamera = new Camera();
	mEditorCamera->LookAt(Vector3(0.0f, 8.0f, -8.0f), Vector3(0, 0, 0), Vector3(0, 1.0f, 0));
	mEditorCamera->IsEditorCamera = true;
	mCameraVec.push_back(mEditorCamera);
	mScene->mEditorCamera = mEditorCamera;

	// 폰트 생성
	D3D11Render* instRender = BTGameEngine::GetInstance()->GetRender();
	mpFont->Create(instRender->GetDevice(), instRender->GetSolidRS(), instRender->GetNormalDSS());

	// DirectionalLight (최대 3개)
	DirectionalLight* dirLight = new DirectionalLight();
	dirLight->Ambient	= Vector4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight->Diffuse	= Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	dirLight->Specular  = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight->Direction = Vector3(0, 0, 1.f);
	mDirLightVec.push_back(*dirLight);

	//DirectionalLight* dirLight2 = new DirectionalLight();
	//dirLight2->Ambient	 = Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	//dirLight2->Diffuse	 = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	//dirLight2->Specular  = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	//dirLight2->Direction = Vector3(0, -1.f, 0);
	//mDirLightVec.push_back(*dirLight2);

	delete dirLight;
	//delete dirLight2;

	// AxisObject
	GameObject* axisObject = new GameObject();
	HelperRenderer* axisHelper = axisObject->AddComponent<HelperRenderer>();
	axisHelper->SetMeshDatasByStr(L"Axis");

	// GridObject
	GameObject* gridObject = new GameObject();
	HelperRenderer* gridHelper = gridObject->AddComponent<HelperRenderer>();
	gridHelper->SetMeshDatasByStr(L"Grid");
}

EngineScene::~EngineScene()
{
	SafeDelete(mpFont);
}

void EngineScene::Update(float deltaTime)
{
	if (!mScene->IsEnable) { return; }
	Object::Scene = this;


	mEditorCamera->CameraMove(deltaTime);
	mScene->Update(deltaTime);

	// 카메라들 Update 후 UpdateViewMatrix 하도록 해야함 
	for (int index = 0; index < mCameraVec.size(); ++index)
	{
		mCameraVec[index]->UpdateViewMatrix();
	}

	// 게임 오브젝트 모두 Update / 카메라도 Update 하도록 해야하낭?
	for (int index = 0; index < mGameObjectVec.size(); ++index)
	{
		mGameObjectVec[index]->Update(deltaTime);
	}

	for (int index = 0; index < mRendererVec.size(); ++index)
	{
		mRendererVec[index]->UpdateRenderer(mEditorCamera);
	}
}

void EngineScene::Render()
{
	// 렌더러 오브젝트 모두 그리기	
	for (int index = 0; index < mRendererVec.size(); ++index)
	{
		mRendererVec[index]->Render(mEditorCamera);
	}

	// 카메라가 그려야하는 ( 스카이박스 )
	for (int index = 0; index < mCameraVec.size(); ++index)
	{
		mCameraVec[index]->Render();
	}

	// 카메라 위치와 방향 출력
	Vector3 pos = mEditorCamera->GetPos();
	Vector3 look = mEditorCamera->GetLook();
	mpFont->DrawTextColor(10, 10, Vector4(1.f, 1.f, 1.f, 1.f),
		(TCHAR*)L"Pos : %f, %f, %f \nLook : %f, %f, %f",
		pos.x, pos.y, pos.z, look.x, look.y, look.z);

	mpFont->DrawTextColor(10, 50, Vector4(1.0f, 1.0f, 0.0f, 1.0f)
	,(TCHAR*)L"[왼쪽 Joe]\nPlay(Loop)\t:\tJ\nPause\t\t\t:\tN\nResuem\t\t:\tU\nStop\t\t\t\t:\tY");
	mpFont->DrawTextColor(10, 150, Vector4(1.0f, 0.0f, 1.0f, 1.0f)
		, (TCHAR*)L"[오른쪽 Joe]\nPlay(delay 3f)\t:\tK");

}
