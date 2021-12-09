#pragma once

namespace BTEngine
{
	bool Init(int sWidth, int sHeight, bool vsync, HWND hWnd, bool fullscreen, float sDepth, float sNear);
	
	void OnResize(int sWidth, int sHeight);

	void EngineUpdate(float deltaTime);

}

namespace BTInputEvent
{
	bool InputKey(int vk);
	bool InputKeyDown(int vk);
	bool InputKeyUp(int vk);

	POINT GetMousePosCT();
	POINT GetMousePosSC();
}

namespace BTLoader
{
	void LoadModelDataSet(std::wstring name, std::wstring pFilePath);
	void LoadAnimation(std::wstring name, std::wstring pFilePath);
	void LoadTextureFromDDS(std::wstring name, std::wstring pFilePath);
	void LoadTexturePng(std::wstring name, std::wstring pFilePath);
}


class BTMaterial;
namespace BTResource
{
	// Material 정보 획득
	BTMaterial* GetMaterial(std::wstring name, int refNum);

	// Material에 MapResource 설정
	void SetDiffuseMapOnMat(BTMaterial* targetMat, std::wstring diffuseName);
	void SetNormalMapOnMat(BTMaterial* targetMat, std::wstring normalName);
	// Effect설정도 필요

	// MeshData에 Material 설정
	void SetMatOnModelData(std::wstring name, int meshNum, BTMaterial* mat);
	void SetMatOnModelDataSet(std::wstring name, BTMaterial* mat);

}

namespace BTScene
{
	void CreateScene(IScene* pScene);
} 