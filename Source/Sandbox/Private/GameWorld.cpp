#include "GameWorld.h"

#include <StandardTypes/StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <Logger/Logger.h>
#include <Timer/Timer.h>

#include <Input/InputMapper.h>

#include "MoveCubeComponent.h"
#include "ESystemManager.h"
#include "Scene/SceneManager.h"
#include "MoveCubeSystem.h"
#include "Systems/StaticMeshRenderSystem.h"
#include <Components/ETransform.h>
#include <Components/EStaticMesh.h>
#include <Components/ENameComponent.h>


#include "RenderAssets/Texture2D.h"
#include "RenderAssets/Material.h"


#include "Renderer.h"

#include <Archive/YamlArchive.h>

namespace {
	GameWorld gameworld;

	//Vertex verticies[] =
	//{
	//	// Front face
	//{ { -1, -1, -1, 1 }, { 1, 0, 0 } }, // Bottom-left-front (red)
	//{ { -1,  1, -1, 1 }, { 1, 0, 0 } }, // Top-left-front
	//{ {  1,  1, -1, 1 }, { 1, 0, 0 } }, // Top-right-front
	//{ {  1, -1, -1, 1 }, { 1, 0, 0 } }, // Bottom-right-front

	//// Back face
	//{ { -1, -1,  1, 1 }, { 0, 1, 0 } }, // Bottom-left-back (green)
	//{ {  1, -1,  1, 1 }, { 0, 1, 0 } }, // Bottom-right-back
	//{ {  1,  1,  1, 1 }, { 0, 1, 0 } }, // Top-right-back
	//{ { -1,  1,  1, 1 }, { 0, 1, 0 } }, // Top-left-back

	//// Left face
	//{ { -1, -1,  1, 1 }, { 0, 0, 1 } }, // Bottom-left-back (blue)
	//{ { -1,  1,  1, 1 }, { 0, 0, 1 } }, // Top-left-back
	//{ { -1,  1, -1, 1 }, { 0, 0, 1 } }, // Top-left-front
	//{ { -1, -1, -1, 1 }, { 0, 0, 1 } }, // Bottom-left-front

	//// Right face
	//{ { 1, -1, -1, 1 }, { 1, 1, 0 } }, // Bottom-right-front (yellow)
	//{ { 1,  1, -1, 1 }, { 1, 1, 0 } }, // Top-right-front
	//{ { 1,  1,  1, 1 }, { 1, 1, 0 } }, // Top-right-back
	//{ { 1, -1,  1, 1 }, { 1, 1, 0 } }, // Bottom-right-back

	//// Top face
	//{ { -1, 1, -1, 1 }, { 1, 0, 1 } }, // Top-left-front (magenta)
	//{ { -1, 1,  1, 1 }, { 1, 0, 1 } }, // Top-left-back
	//{ {  1, 1,  1, 1 }, { 1, 0, 1 } }, // Top-right-back
	//{ {  1, 1, -1, 1 }, { 1, 0, 1 } }, // Top-right-front

	//// Bottom face
	//{ { -1, -1, -1, 1 }, { 0, 1, 1 } }, // Bottom-left-front (cyan)
	//{ {  1, -1, -1, 1 }, { 0, 1, 1 } }, // Bottom-right-front
	//{ {  1, -1,  1, 1 }, { 0, 1, 1 } }, // Bottom-right-back
	//{ { -1, -1,  1, 1 }, { 0, 1, 1 } }, // Bottom-left-back
	//};

	uint16_t cubeIndices[] = {
		// Front face
		0, 1, 2,
		0, 2, 3,

		// Back face
		4, 5, 6,
		4, 6, 7,

		// Left face
		8, 9, 10,
		8, 10, 11,

		// Right face
		12, 13, 14,
		12, 14, 15,

		// Top face
		16, 17, 18,
		16, 18, 19,

		// Bottom face
		20, 21, 22,
		20, 22, 23,
	};
}

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}


REFL_DEFINE(AssetRef<Mesh>)
{
	refl::ClassBuilder<AssetRef<Mesh>>(STRINGIFYEXP(AssetRef<Mesh>), "663c9ab4-e3eb-4e39-89ee-a28f365ce91c")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				SaveAssetRef<Mesh>(obj, archive, key);
			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				LoadAssetRef<Mesh>(obj, archive, key);
				return true;
			});
}

void GameWorld::Init()
{
	LOG_INFO("GameWorld init...");

	AssetRegistry* assetReg = AssetRegistry::Instance();
	SceneManager* sceMan = SceneManager::Instance();

	////Ent 1
	//AssetRef<Mesh> asset = assetReg->Load<Mesh>("engine://Models/sm_cube.asset");
	//for (size_t i = 0; i < 1; i++)
	//{
	//	EEntity ent1 = sceMan->GetActiveScene()->GetWorld().CreateEntity();

	//	ENameComponent* name1 = sceMan->GetActiveScene()->GetWorld().AddComponent<ENameComponent>(ent1);
	//	name1->name = "Cube";

	//	sceMan->GetActiveScene()->GetWorld().AddComponent<ETransform>(ent1);
	//	
	//	MoveCubeComponent* move = sceMan->GetActiveScene()->GetWorld().AddComponent<MoveCubeComponent>(ent1);
	//	move->speedMultiplier = RandomFloat(0.2f, 10.f);
	//	move->distance = RandomFloat(5.f, 100.f);

	//	EStaticMesh* sMesh1 = sceMan->GetActiveScene()->GetWorld().AddComponent<EStaticMesh>(ent1);
	//	sMesh1->mesh = asset;
	//}

	////Ent 2
	//EEntity ent2 = sceMan->GetActiveScene()->GetWorld().CreateEntity();

	//ENameComponent* name2 = sceMan->GetActiveScene()->GetWorld().AddComponent<ENameComponent>(ent2);
	//name2->name = "Player";

	//ETransform* trans = sceMan->GetActiveScene()->GetWorld().AddComponent<ETransform>(ent2);
	//Vector3 newScale = trans->transform.GetScale() * 0.01f;
	//trans->transform.SetScale(newScale);

	//AssetRef<Mesh> asset2 = assetReg->Load<Mesh>("engine://Models/sm_player.asset");
	//EStaticMesh* sMesh2 = sceMan->GetActiveScene()->GetWorld().AddComponent<EStaticMesh>(ent2);
	//sMesh2->mesh = asset2;

	//Systems
	ESystemManager* sysMan = ESystemManager::Instance();
	sysMan->RegisterSystem(&MoveCubeSystem, "Move Cube", EPipeline::ESystemUpdate);
	sysMan->RegisterSystem(&StaticMeshRenderSystem, "Static Mesh Render", EPipeline::ESystemPostUpdate);

	//sceMan->SaveActiveSceneToFile("engine://Test.scene");
	sceMan->LoadScene("Test");
	sceMan->SetActiveScene("Test");
}

void GameWorld::Render()
{
}

void InitGameWorld()
{
	gameworld.Init();
}

void RenderGameWorld()
{
	gameworld.Render();
}
