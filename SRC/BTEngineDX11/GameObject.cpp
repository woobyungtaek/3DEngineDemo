#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	mTransform = new Transform();
	Object::Scene->mGameObjectVec.push_back(this);
}

GameObject::~GameObject()
{
}

void GameObject::Update(float deltaTime)
{
	int count = (int)mComponents.size();
	for (int index = 0; index < count; ++index)
	{
		mComponents[index]->Update(deltaTime);
	}
}
