#pragma once
class TestScene : public IScene
{
public:
	TestScene();
	~TestScene();

private:
	//생성한 GameObject들
	GameObject* mJoe1;
	GameObject* mJoe2;
	GameObject* mGenji;
	GameObject* mBox;
	GameObject* mGrim;
	GameObject* mGrim2;

	// 테스트용 Start vlaue
	bool IsStarted;

public:
	virtual void Start(float deltaTime) override;
	virtual void Update(float deltaTime) override;
	virtual void End(float deltaTime) override;
};