#pragma once

using namespace std;

class BTBehaviour;

class GameObject : public Object
{
public:
	GameObject();
	~GameObject();

public:
	Transform* mTransform;
	vector<BTBehaviour*> mComponents;

public:
	void Update(float deltaTime);

public:
	template<typename T>
	T* GetComponent();

	template<typename T>
	T* AddComponent();

	template<typename T>
	bool RemoveComponent();
};

// 컴포넌트를 획득합니다.(현재는 BTHaviour)
template<typename T>
inline T* GameObject::GetComponent()
{
	string key = string(typeid(T).name());

	int count = (int)mComponents.size();

	for (int index = 0; index < count; ++index)
	{
		T* temp = dynamic_cast<T*>(mComponents[index]);
		if (temp != nullptr)
		{
			return temp;
		}
	}

	return nullptr;
}

// 컴포넌트를 추가, 획득합니다.(현재는 BTHaviour)
template<typename T>
inline T* GameObject::AddComponent()
{
	string key = string(typeid(T).name());

	int count = (int)mComponents.size();
	for (int index = 0; index < count; ++index)
	{
		if (mComponents[index]->componentName == key)
		{
			// 중복 컴포넌트는 추가 불가
			return nullptr;
		}
	}

	T* instT = new T();

	BTBehaviour* tempBehaviour = static_cast<BTBehaviour*>(instT);
	tempBehaviour->componentName = key;
	tempBehaviour->gameObject = this;

	mComponents.push_back(instT);

	return instT;
}

// 컴포넌트를 제거합니다.(현재는 BTHaviour)
template<typename T>
inline bool GameObject::RemoveComponent()
{
	string key = string(typeid(T).name());

	vector< BTBehaviour*>::iterator iter;

	for (iter = mComponents.begin(); iter != mComponents.end(); ++iter)
	{
		if ((*iter)->componentName == key)
		{
			mComponents.erase(iter);
			return true;
		}
	}
	return false;
}
