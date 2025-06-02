#pragma once
#include <Core.h>

#include <EWorld.h>

class Scene
{
public:
	Scene();
	Scene(const String& aName);
	~Scene();

	bool Init();

	const EWorld& GetEWorld();
	const String& GetName() const;

private:
	EWorld mECSWorld;
	String mName;

};