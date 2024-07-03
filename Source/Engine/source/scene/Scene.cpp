#include "EnginePch.h"
#include "Scene.h"

Scene::Scene()
	:
	mName(""),
	mId(0)
{
}

Scene::Scene(uint32_t aId)
	:
	mName(""),
	mId(aId)
{
}

Scene::Scene(uint32_t aId, const std::string& aName)
	:
	mName(aName),
	mId(aId)
{
}

Scene::~Scene()
{
}

ecs::World& Scene::GetWorld()
{
	return mWorld;
}

void Scene::SetName(const std::string& aName)
{
	mName = aName;
}

uint32_t Scene::GetID() const
{
	return mId;
}
