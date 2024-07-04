#include "GamePch.h"
#include "GameWorld.h"

#include <Window.h>
#include <Camera.h>

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	ecs::World& currentWorld = Engine::GetSceneManager().CreateScene().GetWorld();
	ecs::Entity entity = currentWorld.CreateEntity();

	{
		ecs::Transform* transform = currentWorld.GetComponent<ecs::Transform>(entity);
		transform->position = {0,0,3};

		ecs::Mesh mesh;
		mesh.meshPath = L"adadadaaddd";
		mesh.modelInstance = Engine::GetGraphicsEngine().GetModelFactory().GetModelInstance(L"../EngineAssets/Models/sm_cube.fbx");
		currentWorld.AddComponent(entity, mesh);
	}

	RegisterSystems();
}

void GameWorld::RegisterSystems()
{

}
