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
	//ecs::World& currentWorld = Engine::GetSceneManager().CreateScene().GetWorld();
	//ecs::Entity entity = currentWorld.CreateEntity();

	//{
	//	ecs::Mesh mesh;
	//	mesh.meshPath = L"adadadaaddd";
	//	mesh.modelInstance = Engine::GetGraphicsEngine().GetModelFactory().GetModelInstance(L"../EngineAssets/Models/sm_cube.fbx");
	//	currentWorld.AddComponent(entity, mesh);
	//}

	RegisterSystems();
}

void GameWorld::RegisterSystems()
{

}
