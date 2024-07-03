#pragma once
#include "Scene.h"

using SceneID = uint32_t;

class SceneManager
{
public:
	ecs::World& GetCurrentEcsWorld();

	void OnInit();
	void OnUpdate();

	void SetActiveScene(uint32_t aId);
	Scene& CreateScene(uint32_t aId = 0, const std::string& aName = "Unnamed");
	Scene& GetCurrentScene();

private:
	friend class Engine;
	SceneManager() = default;
	~SceneManager() = default;
private:
	SceneID mActiveScene = 0;
	SceneID mNextSceneID = 0; 
	std::unordered_map<SceneID, Scene> mScenes;
};