#pragma once
#include <Core.h>

#include <EWorld.h>

class Scene
{
public:
	Scene();
	Scene(const std::string& aName);
	~Scene();

	bool Init();

	const EWorld& GetEWorld();
	const std::string& GetName() const;

private:
	EWorld mECSWorld;
	std::string mName;

};