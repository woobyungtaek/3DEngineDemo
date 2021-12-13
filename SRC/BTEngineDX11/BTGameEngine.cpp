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

// 기능
void BTGameEngine::EngineUpdate(float deltaTime)
{
	int enegineSceneCount = (int)mEnegineSceneVec.size();

	// 씬 전체 업데이트
	for (int index = 0; index < enegineSceneCount; ++index)
	{
		// Active 상태 확인
		mEnegineSceneVec[index]->Update(deltaTime);
	}

	// 씬 전체 렌더링
	mRender->BeginRender(0.05f,0.05f,0.05f,1.0f);

	for (int index = 0; index < enegineSceneCount; ++index)
	{
		// 기본 실행
		mRender->BeginSceneRender();

		// 씬 구성 요소들 Rendering
		mEnegineSceneVec[index]->Render();

		// 기본 실행
		mRender->EndSceneRender();

		// 라이팅 씬별로 해서 결과물이 따로 저장되어 있어야함
		// 디퍼드 적용시 EndRender(); 이후에 LightRender가 되어야함 
		// 위에서 모든 결과가 적용되고 빛 연산후 마지막에 백버퍼에서 최종그리기?
	}

	// 씬별 결과물을 조합 하는... (어쩌자고 다 나눠놔서 난리가 났구만)
	 
	// 렌더링 끝 최종
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
