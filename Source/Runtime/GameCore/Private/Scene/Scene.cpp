#include "Scene/Scene.h"

#include <ESystemManager.h>
#include "Scene/SceneManager.h"

Scene::Scene() 
	:
	mName("UNDEFINED")
{
}

Scene::Scene(SceneManager* Manager, const UniqueID128& UUID)
	:
	mUUID(UUID),
	mName("UNDEFINED"),
	mManager(Manager)
{
}

Scene::Scene(SceneManager* Manager, const UniqueID128& UUID, const String& Name)
	:
	mUUID(UUID),
	mName(Name),
	mManager(Manager)
{
}

Scene::Scene(SceneManager* Manager, const UniqueID128& UUID, const String& Name, const Path& SourcePath)
	:
	mUUID(UUID),
	mName(Name),
	mSourcePath(SourcePath),
	mManager(Manager)
{
}

Scene::~Scene()
{
}

void Scene::OnLoad()
{
	ESystemManager* sysMan = ESystemManager::Instance();
	sysMan->RunLoad(mEWorld, mManager->GetExecutionPhase());
}

void Scene::OnUpdate()
{
	mSceneView.Reset();

	ESystemManager* sysMan = ESystemManager::Instance();
	sysMan->RunUpdate(mEWorld, mManager->GetExecutionPhase());
	
	mSceneView.Flush();
}

GAMECORE_API void Scene::Unload()
{
	mEWorld = EWorld();
}

GAMECORE_API Scene Scene::Clone()
{
	Scene clone;

	clone.mEWorld = mEWorld;
	clone.mSceneView = mSceneView;
	clone.mSourcePath = mSourcePath;
	clone.mManager = mManager;

	return clone;
}
