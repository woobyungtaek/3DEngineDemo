/*
	EngineScene : �������� Render�� Game�� �̾��ִ� ����, Object, Light, Camera���� ��� �˰� �ִ´�.
	IScene : �ܺο��� ������ �ؾ��ϴ� Scene ����
	Render�� EngineScene�� Render()�� ����
*/
#pragma once

class EngineScene
{
public:
	EngineScene(IScene* scene);
	~EngineScene();

public:
	// ������Ʈ ��
	void Update(float deltaTime);
	void Render();

private:
	// Update�� �����ǵ� Scene ��ü
	IScene* mScene; // = TestScene 

	// ��Ʈ
	DXTKFont* mpFont;

	// ������ ī�޶�
	Camera* mEditorCamera;

public:
	// ������Ʈ List
	std::vector<GameObject*> mGameObjectVec;

	// �������� �����ִ� �ֵ��� ���� ����?
	// ������ List ( ���� ���� )
	std::vector<Renderer*> mRendererVec;

	// ī�޶�	List
	std::vector<Camera*> mCameraVec;	

	// ����Ʈ	List
	//std::vector<DirectionalLight> mDirLightVec;
	std::vector<DirectionalLight> mDirLightVec;
	//DirectionalLight mDirLightVec[3];

	// �۾� ��� ���� List

};

