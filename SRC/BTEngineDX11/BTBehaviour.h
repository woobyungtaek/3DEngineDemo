#pragma once
class GameObject;

class BTBehaviour : public Object
{
public:
	BTBehaviour() { }
	virtual ~BTBehaviour() {}

public:	
	GameObject* gameObject; // 현재 속해있는 GameObject

	std::string componentName; // Component 이름

public:
	virtual void Start() {};
	virtual void Update(float deltaTime) {};
};

