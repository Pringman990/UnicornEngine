#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
}

Scene::Scene(const String& aName)
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

const String& Scene::GetName() const
{
	return mName;
}
