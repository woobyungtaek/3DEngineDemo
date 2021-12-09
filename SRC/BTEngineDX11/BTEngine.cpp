#include "pch.h"

// 게임 엔진
namespace BTEngine
{
	bool BTEngine::Init(int sWidth, int sHeight, bool vsync, HWND hWnd, bool fullscreen, float sDepth, float sNear)
	{
		// 랜더 생성 및 초기화
		bool result = BTGameEngine::GetInstance()->GetRender()->Init(sWidth, sHeight, vsync, hWnd, fullscreen, sDepth, sNear);
		if (!result) { return false; }

		// 입력 이벤트 초기화
		BTInput::Init(hWnd);

		// 리소스 매니저 초기화
		ResourceManager::GetInstance()->Init();

		return true;
	}

	void BTEngine::OnResize(int sWidth, int sHeight)
	{
		BTGameEngine::GetInstance()->GetRender()->OnResize(sWidth, sHeight);
		BTGameEngine::GetInstance()->ResizeAllScene(sWidth, sHeight);
	}

	void BTEngine::EngineUpdate(float deltaTime)
	{
		BTInput::KeyUpdate();
		BTInput::MouseUpdate();

		BTGameEngine::GetInstance()->EngineUpdate(deltaTime);
	}

}

namespace BTLoader
{
	// Model 정보 로드 ( 애니메이션 제외 )
	void BTLoader::LoadModelDataSet(std::wstring name, std::wstring pFilePath)
	{
		ResourceManager::GetInstance()->LoadModelDataSet(name, pFilePath);
	}

	// Animation 정보 로드
	void BTLoader::LoadAnimation(std::wstring name, std::wstring pFilePath)
	{
		ResourceManager::GetInstance()->LoadAnimation(name, pFilePath);
	}

	void BTLoader::LoadTextureFromDDS(std::wstring name, std::wstring pFilePath)
	{
		ResourceManager::GetInstance()->LoadTextureFromDDS(name, pFilePath);
	}

	void BTLoader::LoadTexturePng(std::wstring name, std::wstring pFilePath)
	{
		ResourceManager::GetInstance()->LoadTextureFromPng(name, pFilePath);
	}
}

// 리소스 관련
namespace BTResource
{
	BTMaterial* BTResource::GetMaterial(wstring name, int refNum)
	{
		return ResourceManager::GetInstance()->GetMaterial(name, refNum);
	}

	void BTResource::SetDiffuseMapOnMat(BTMaterial* targetMat, std::wstring diffuseName)
	{
		targetMat->mDiffuseMap = ResourceManager::GetInstance()->GetTexture(diffuseName);
	}
	void BTResource::SetNormalMapOnMat(BTMaterial* targetMat, std::wstring normalName)
	{
		targetMat->mNormalMap = ResourceManager::GetInstance()->GetTexture(normalName);
	}

	void SetMatOnModelData(std::wstring name, int meshNum, BTMaterial* mat)
	{
		ModelDataSet* dataSet = ResourceManager::GetInstance()->GetModelDatasOrginal(name);

		int count = (int)dataSet->mModelDataVec.size();

		if (meshNum >= count) { return; }

		dataSet->mModelDataVec[meshNum]->mMaterial = mat;
	}
	void SetMatOnModelDataSet(std::wstring name, BTMaterial* mat)
	{
		ModelDataSet* dataSet = ResourceManager::GetInstance()->GetModelDatas(name);

		int count = (int)dataSet->mModelDataVec.size();
		for (int index = 0; index < count; ++index)
		{
			dataSet->mModelDataVec[index]->mMaterial = mat;
		}
	}
}

// 씬 관련
namespace BTScene
{
	void BTScene::CreateScene(IScene* pScene)
	{
		EngineScene* instScene = new EngineScene(pScene);
		BTGameEngine::GetInstance()->AddEngineScene(instScene);
	}
}

// 입력 관련
namespace BTInputEvent
{
	bool BTInputEvent::InputKey(int vk)
	{
		return BTInput::InputKey(vk);
	}

	bool BTInputEvent::InputKeyDown(int vk)
	{
		return BTInput::InputKeyDown(vk);
	}

	bool BTInputEvent::InputKeyUp(int vk)
	{
		return BTInput::InputKeyUp(vk);
	}

	POINT BTInputEvent::GetMousePosCT()
	{
		return BTInput::GetMousePosCT();
	}

	POINT BTInputEvent::GetMousePosSC()
	{
		return BTInput::GetMousePosSC();
	}
}
