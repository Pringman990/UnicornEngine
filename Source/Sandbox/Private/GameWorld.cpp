#include "GameWorld.h"

#include <StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <Logger.h>
#include <Renderer.h>
#include <MeshLoading/MeshResourceManager.h>
#include <ResourceRegistry.h>

namespace {
	GameWorld gameworld;
}

GameWorld::GameWorld()
{
}
  
GameWorld::~GameWorld()
{
	mMeshes.clear();
}

void GameWorld::Init()
{
	LOG_INFO("GameWorld init..."); 
	MeshResourceManager* meshManager = GET_RESOURCE_MANAGER(MeshResourceManager); 
	std::vector<Mesh*> meshes = meshManager->LoadMeshesFromFile("../../Assets/Models/sm_cube.fbx");

	for (uint32 x = 0; x < 100; x++)
	{
		for (uint32 y = 0; y < 100; y++)
		{
			Transform trans;
			trans.SetPosition(Vector3((float)x, 0.f, (float)y));
			mMeshes.push_back({ meshes[0], trans });
		}
	}
}

void GameWorld::Render()
{
	RenderCommandList commandList;
	for (uint32 i = 0; i < mMeshes.size(); i++)
	{
		commandList.RecordDrawMesh(mMeshes[i].first, mMeshes[i].second);
	}
	
	RenderPass pass;
	pass.AddCommandList(commandList);
	
	Renderer::GetInstance()->GetRenderQueue()->SubmitRenderPass(pass);
}

void InitGameWorld()
{
	gameworld.Init();
}

void RenderGameWorld()
{
	gameworld.Render();
}
