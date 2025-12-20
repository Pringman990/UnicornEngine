#include "Scene/Scene.h"

#include <ESystemManager.h>

Scene::Scene() 
	:
	mName("UNDEFINED")
{
}

Scene::Scene(const UniqueID128& UUID)
	:
	mUUID(UUID),
	mName("UNDEFINED")
{
}

Scene::Scene(const UniqueID128& UUID, const String& Name)
	:
	mUUID(UUID),
	mName(Name)
{
}

Scene::Scene(const UniqueID128& UUID, const String& Name, const Path& SourcePath)
	:
	mUUID(UUID),
	mName(Name),
	mSourcePath(SourcePath)
{
}

Scene::~Scene()
{
}

void Scene::OnLoad()
{
	ESystemManager* sysMan = ESystemManager::Instance();
	sysMan->RunLoad(mEWorld);
}

void Scene::OnUpdate()
{
	ESystemManager* sysMan = ESystemManager::Instance();
	sysMan->RunUpdate(mEWorld);
}

GAMECORE_API void Scene::Unload()
{
	mEWorld = EWorld();
}
