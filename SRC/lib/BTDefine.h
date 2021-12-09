#pragma once

class IScene
{
public:
	IScene() : IsEnable(false) {};

public:
	// 추상 메서드
	virtual void Start(float deltaTime) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void End(float deltaTime) = 0;

public:
	// 활성화 상태
	bool IsEnable;

	Camera* mEditorCamera;
};