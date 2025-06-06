#pragma once
#include <Core.h>

#include <EWorld.h>

class Scene
{
public:
	Scene();
	Scene(const String& Name);
	~Scene();

	bool Init();

	const EWorld& GetEWorld();
	const String& GetName() const;

private:
	EWorld mECSWorld;
	String mName;

};