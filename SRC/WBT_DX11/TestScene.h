#pragma once
class TestScene : public IScene
{
public:
	TestScene();
	~TestScene();

private:
	//������ GameObject��
	GameObject* mJoe1;
	GameObject* mJoe2;
	GameObject* mGenji;
	GameObject* mBox;
	GameObject* mGrim;
	GameObject* mGrim2;

	// �׽�Ʈ�� Start vlaue
	bool IsStarted;

public:
	virtual void Start(float deltaTime) override;
	virtual void Update(float deltaTime) override;
	virtual void End(float deltaTime) override;
};