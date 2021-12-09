#pragma once

class IScene
{
public:
	IScene() : IsEnable(false) {};

public:
	// �߻� �޼���
	virtual void Start(float deltaTime) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void End(float deltaTime) = 0;

public:
	// Ȱ��ȭ ����
	bool IsEnable;

	Camera* mEditorCamera;
};