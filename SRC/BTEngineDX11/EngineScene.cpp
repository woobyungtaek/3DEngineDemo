
#include "pch.h"
#include "EngineScene.h"

EngineScene::EngineScene(IScene* scene)
{
	mScene = scene;
	Object::Scene = this;

	// ��Ʈ
	if (mpFont == nullptr)
	{
		mpFont = new DXTKFont();
	}

	// ������ ī�޶�
	mEditorCamera = new Camera();
	mEditorCamera->LookAt(Vector3(0.0f, 8.0f, -8.0f), Vector3(0, 0, 0), Vector3(0, 1.0f, 0));
	mEditorCamera->IsEditorCamera = true;
	mCameraVec.push_back(mEditorCamera);
	mScene->mEditorCamera = mEditorCamera;

	// ��Ʈ ����
	D3D11Render* instRender = BTGameEngine::GetInstance()->GetRender();
	mpFont->Create(instRender->GetDevice(), instRender->GetSolidRS(), instRender->GetNormalDSS());

	// DirectionalLight (�ִ� 3��)
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

	// ī�޶�� Update �� UpdateViewMatrix �ϵ��� �ؾ��� 
	for (int index = 0; index < mCameraVec.size(); ++index)
	{
		mCameraVec[index]->UpdateViewMatrix();
	}

	// ���� ������Ʈ ��� Update / ī�޶� Update �ϵ��� �ؾ��ϳ�?
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
	// ������ ������Ʈ ��� �׸���	
	for (int index = 0; index < mRendererVec.size(); ++index)
	{
		mRendererVec[index]->Render(mEditorCamera);
	}

	// ī�޶� �׷����ϴ� ( ��ī�̹ڽ� )
	for (int index = 0; index < mCameraVec.size(); ++index)
	{
		mCameraVec[index]->Render();
	}

	// ī�޶� ��ġ�� ���� ���
	Vector3 pos = mEditorCamera->GetPos();
	Vector3 look = mEditorCamera->GetLook();
	mpFont->DrawTextColor(10, 10, Vector4(1.f, 1.f, 1.f, 1.f),
		(TCHAR*)L"Pos : %f, %f, %f \nLook : %f, %f, %f",
		pos.x, pos.y, pos.z, look.x, look.y, look.z);

	mpFont->DrawTextColor(10, 50, Vector4(1.0f, 1.0f, 0.0f, 1.0f)
	,(TCHAR*)L"[���� Joe]\nPlay(Loop)\t:\tJ\nPause\t\t\t:\tN\nResuem\t\t:\tU\nStop\t\t\t\t:\tY");
	mpFont->DrawTextColor(10, 150, Vector4(1.0f, 0.0f, 1.0f, 1.0f)
		, (TCHAR*)L"[������ Joe]\nPlay(delay 3f)\t:\tK");

}
