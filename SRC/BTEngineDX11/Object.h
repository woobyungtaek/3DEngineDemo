#pragma once
class EngineScene;
class Object
{
public:
	Object() {}
	virtual ~Object() {}

public:
	static EngineScene* Scene;
};

