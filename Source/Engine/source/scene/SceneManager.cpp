#include "EnginePch.h"
#include "SceneManager.h"

ecs::World& SceneManager::GetCurrentEcsWorld()
{
	return mScenes[mActiveScene].GetWorld();
}

void SceneManager::OnInit()
{
	mScenes[mActiveScene].GetWorld().ProcessInitSystems();
}

void SceneManager::OnUpdate()
{
	mScenes[mActiveScene].GetWorld().ProcessUpdateSystems();
}

void SceneManager::SetActiveScene(uint32_t aId)
{
	auto it = mScenes.find(aId);
	if (it != mScenes.end())
	{
		std::cout << "Scene with id: " << std::to_string(aId) << ", does not exist" << std::endl;
		return;
	}

	mActiveScene = aId;
	OnInit();
}

Scene& SceneManager::CreateScene(uint32_t aId, const std::string& aName)
{
	if (aId != 0)
	{
		auto it = mScenes.find(aId);
		if (it != mScenes.end())
		{
			std::cout << "Scene id already exists, was this intended?" << std::endl;
		}
		return mScenes[aId] = Scene(aId, aName);
	}
	uint32_t sceneID = mNextSceneID++;
	return mScenes[sceneID] = Scene(sceneID, aName);
}

Scene& SceneManager::GetCurrentScene()
{
	return mScenes[mActiveScene];  
}
