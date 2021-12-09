/*
	모든 모듈을 관리하는 가장 큰 부분

	렌더러, 사운드, 기타등등 기능 엔진이 포함 되어야한다.

	화면을 구성하는 가장 큰 단위인 Scene을 관리한다.
*/

#pragma once
class BTGameEngine
{
private:
	static BTGameEngine* mInstance;
	BTGameEngine();

public:
	static BTGameEngine* GetInstance();
	~BTGameEngine();

public:
	D3D11Render* GetRender();

private:
	// D3D11 랜더러
	D3D11Render* mRender;

	// Scene Manager가 생겨야하며 인터페이스와
	vector<EngineScene*> mEnegineSceneVec;

public:
	void EngineUpdate(float deltaTime);
	void AddEngineScene(EngineScene* pEngineScene);
	void ResizeAllScene(int sWidth, int sHeight);
};

