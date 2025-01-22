#include "GameWorld.h"

#include <StandardTypes/StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <Logger/Logger.h>
#include <FileWatcher/FileWatcher.h>
#include <Renderer.h>
#include <MeshLoading/MeshResourceManager.h>
#include <AssetRegistry.h>

#include <Texture/Texture.h>
#include <Texture/TextureResourceManager.h>
#include <MaterialResourceManager.h>
#include <Shader/ShaderManager.h>
#include <Lights/AmbientLight.h>

namespace {
	GameWorld gameworld;
	AmbientLight ambLight;
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
	std::string contentPath = FileWatcher::GetInstance()->GetContentPath();

	TextureResourceManager* textureManager = GET_ASSET_MANAGER(TextureResourceManager);
	TextureCube* cubemap = (TextureCube*)textureManager->LoadTextureFromFile(contentPath + "/Textures/cube_1024_preblurred_angle3_Skansen3.dds");
	ambLight.SetCubemap(cubemap);

	MeshResourceManager* meshManager = GET_ASSET_MANAGER(MeshResourceManager);
	std::vector<Mesh*> wall = meshManager->LoadMeshesFromFile(contentPath + "/Models/sm_player.fbx");
	std::vector<Mesh*> cube = meshManager->LoadMeshesFromFile(contentPath + "/Models/sm_cube.fbx");
	std::vector<Mesh*> skySphere = meshManager->LoadMeshesFromFile(contentPath + "/Models/sm_SkySphere.fbx");

	MaterialResourceManager* materialManager = GET_ASSET_MANAGER(MaterialResourceManager);
	Material* skyMaterial = materialManager->CreateMaterial("SkySphereMaterial");
	skyMaterial->AddTexture(0, cubemap);

	ShaderManager* shaderManager = Renderer::GetInstance()->GetShaderManager();
	MaterialShader* shader = shaderManager->TryGetMaterialShader("../../Source/Runtime/Renderer/RawShaders/Public/Sky_Sphere_PS.hlsl", "../../Source/Runtime/Renderer/RawShaders/Public/Lit_Model_VS.hlsl");
	skyMaterial->SetShader(shader);


	skySphere[0]->SetMaterial(0, skyMaterial);

	for (int32 i = 0; i < 1; i++)
	{
		Transform trans;
		trans.SetPosition(Vector3((float)i, 0.f, (float)(i + 1)));
		mMeshes.push_back({ wall[0], trans });
		mMeshes.push_back({ cube[0], trans });
		mMeshes.push_back({ skySphere[0], Transform(Vector3(0,0,0), Vector3(), Vector3(400,400,400)) });
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

	RenderPassAttachment attachment;
	attachment.ambientlights = &ambLight;
	pass.AddAttachment(attachment);

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
