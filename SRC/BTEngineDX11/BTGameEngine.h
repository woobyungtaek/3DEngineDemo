/*
	��� ����� �����ϴ� ���� ū �κ�

	������, ����, ��Ÿ��� ��� ������ ���� �Ǿ���Ѵ�.

	ȭ���� �����ϴ� ���� ū ������ Scene�� �����Ѵ�.
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
	// D3D11 ������
	D3D11Render* mRender;

	// Scene Manager�� ���ܾ��ϸ� �������̽���
	vector<EngineScene*> mEnegineSceneVec;

public:
	void EngineUpdate(float deltaTime);
	void AddEngineScene(EngineScene* pEngineScene);
	void ResizeAllScene(int sWidth, int sHeight);
};
