#include "pch.h"

BTGameEngine* BTGameEngine::mInstance;
BTGameEngine* BTGameEngine::GetInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new BTGameEngine();
	}
	return mInstance;
}

BTGameEngine::BTGameEngine()
{
	mRender = new D3D11Render();
}

BTGameEngine::~BTGameEngine()
{
	delete mRender;
	mRender = nullptr;

	mEnegineSceneVec.clear();
}

// Get & Set Func
D3D11Render* BTGameEngine::GetRender()
{
	if (mRender == nullptr)
	{
		mRender = new D3D11Render();
	}

	return mRender;
}

// ���
void BTGameEngine::EngineUpdate(float deltaTime)
{
	int enegineSceneCount = (int)mEnegineSceneVec.size();

	// �� ��ü ������Ʈ
	for (int index = 0; index < enegineSceneCount; ++index)
	{
		// Active ���� Ȯ��
		mEnegineSceneVec[index]->Update(deltaTime);
	}

	// �� ��ü ������
	mRender->BeginRender(0.05f,0.05f,0.05f,1.0f);

	for (int index = 0; index < enegineSceneCount; ++index)
	{
		// �⺻ ����
		mRender->BeginSceneRender();

		// �� ���� ��ҵ� Rendering
		mEnegineSceneVec[index]->Render();

		// �⺻ ����
		mRender->EndSceneRender();

		// ������ ������ �ؼ� ������� ���� ����Ǿ� �־����
		// ���۵� ����� EndRender(); ���Ŀ� LightRender�� �Ǿ���� 
		// ������ ��� ����� ����ǰ� �� ������ �������� ����ۿ��� �����׸���?
	}

	// ���� ������� ���� �ϴ�... (��¼�ڰ� �� �������� ������ ������)
	 
	// ������ �� ����
	mRender->EndRender();

}

void BTGameEngine::AddEngineScene(EngineScene* pEngineScene)
{
	mEnegineSceneVec.push_back(pEngineScene);
}

void BTGameEngine::ResizeAllScene(int sWidth, int sHeight)
{
	for (int index = 0; index < mEnegineSceneVec.size(); ++index)
	{
		Camera* pCamera;
		int count = (int)mEnegineSceneVec[index]->mCameraVec.size();
		for (int cameraIdx = 0; cameraIdx < count; ++cameraIdx)
		{
			pCamera = mEnegineSceneVec[index]->mCameraVec[cameraIdx];

			pCamera->SetLens(0.25f * MathHelper::Pi, (float)sWidth / (float)sHeight, 1.0f, 1000.0f);
		}
	}
}
