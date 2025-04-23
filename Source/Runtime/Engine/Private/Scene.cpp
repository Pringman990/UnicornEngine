#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
}

Scene::Scene(const std::string& aName)
{
}

Scene::~Scene()
{
}

bool Scene::Init()
{

	return false;
}

const EWorld& Scene::GetEWorld()
{
	return mECSWorld;
}

const std::string& Scene::GetName() const
{
	return mName;
}
