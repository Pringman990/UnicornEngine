#include "GameWorld.h"

#include <StandardTypes/StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <Logger/Logger.h>
#include <FileWatcher/FileWatcher.h>
#include <Renderer.h>
#include <Timer/Timer.h> 
#include <Input/InputMapper.h>

#include "ChunkGenerator.h"

#include <ConstantBuffer/CSConstantBuffer.h>
#include <ConstantBuffer/CommonConstantBuffers.h>
#include <ConstantBuffer/ConstantBuffer.h>
#include <ComputeShader.h>
#include <Voxel.h>
#include <Texture2D.h>
#include <UAVTexture.h>
#include <TextureCube.h>
#include <Mesh.h>
#include <GBuffer.h>
#include <StructuredBuffer.h>
#include <Application/Application.h>
#include <MeshLoader.h>
#include <Threading/ThreadPool.h>

#define CHUNK_COUNT 10

namespace {
	GameWorld gameworld;
	ConstantBuffer colorBuffer;
	ComputeShader shadowShader;
	std::vector<Chunk> chunks;
	std::shared_ptr<Mesh> skySphere;
	std::shared_ptr<Mesh> fullQuad;
	std::shared_ptr<GBuffer> gbuffer;
	std::shared_ptr<UAVTexture> uavTexture;
	StructuredBuffer* gChunkMinMaxBuffer;
	Texture3D* gChunksData;
}

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}
void GameWorld::Init()
{
	LOG_INFO("GameWorld init...");

	float aspect = (16.f / 9.f);
	mCamera.SetPerspective(90, aspect, 0.01f, 1000.f);
	mCamera.GetTransform().SetPosition({ 0,0,-5 });
	Renderer::GetInstance()->SetMainCamera(&mCamera);

	std::vector<std::shared_ptr<Mesh>> meshes;
	if (!MeshLoader::LoadScene("../../Content/Models/sm_SkySphere.fbx", meshes))
		return;

	skySphere = meshes[0];
	auto shader = Shader::Create("../../Binaries/Shaders/Mesh_VS.cso", "../../Binaries/Shaders/Sky_Sphere_PS.cso");
	skySphere->GetMaterial(0)->SetShader(shader);
	skySphere->GetMaterial(0)->SetTexture(0, TextureCube::Create("../../Content/Textures/cube_1024_preblurred_angle3_Skansen3.dds"));
	skySphere->GetTransform().SetScale(Vector3(400,400,400));

	//ThreadPool::GetInstance()->Enqueue(
	//	[]()
	//	{
	//		std::vector<ChunkLoadData*> data(CHUNK_COUNT * CHUNK_COUNT);
	//		for (int x = 0; x < CHUNK_COUNT; x++)
	//		{
	//			for (int z = 0; z < CHUNK_COUNT; z++)
	//			{
	//				data[x * CHUNK_COUNT + z] = ChunkGenerator::GenerateChunkFromPerlin(x * CHUNK_SIZE_XZ, z * CHUNK_SIZE_XZ);
	//			}
	//		}
	//		return data;
	//	},
	//	[](std::vector<ChunkLoadData*> ChunkData)
	//	{
	//		for (int32 i = 0; i < ChunkData.size(); i++)
	//		{
	//			Chunk chunk;
	//			chunk.cube = Mesh::CreateCube();
	//			chunk.cube->GetTransform().SetPosition(ChunkData[i]->position);
	//			chunk.cube->GetTransform().SetScale(ChunkData[i]->scale);
	//			chunk.cube->GetMaterial(0)->SetTexture(0, Texture3D::Create(ChunkData[i]->voxelData));
	//			delete ChunkData[i];
	//			chunks.push_back(chunk);
	//		}
	//	}
	//);

	std::vector<ChunkLoadData*> data(CHUNK_COUNT * CHUNK_COUNT);
	for (int x = 0; x < CHUNK_COUNT; x++)
	{
		for (int z = 0; z < CHUNK_COUNT; z++)
		{
			data[x * CHUNK_COUNT + z] = ChunkGenerator::GenerateChunkFromPerlin(x * CHUNK_SIZE_XZ, z * CHUNK_SIZE_XZ);
		}
	}

	for (int32 i = 0; i < data.size(); i++)
	{
		Chunk chunk;
		chunk.cube = Mesh::CreateCube();
		chunk.cube->GetTransform().SetPosition(data[i]->position);
		chunk.cube->GetTransform().SetScale(data[i]->scale);
		Texture3D* texture = Texture3D::Create(data[i]->voxelData);
		chunk.cube->GetMaterial(0)->SetTexture(0, texture);
		chunk.voxelsTexture = texture;
		delete data[i];
		chunks.push_back(chunk);
	}

	MaterialConstantBufferData colorData;
	colorData.colors[1] = float4(0, 1, 0, 1);
	colorData.colors[2] = float4(0.6f, 0.3f, 0, 1);
	colorData.colors[3] = float4(0, 1.0f, 1, 1);
	colorData.colors[4] = float4(1, 0.79f, 0.29f, 1);
	colorBuffer.Init(sizeof(MaterialConstantBufferData), &colorData);

	IWindowInfo& windowInfo = Application::GetInstance()->GetApplication()->GetWindowInfo();
	gbuffer = GBuffer::Create(Vector2((float)windowInfo.windowWidth, (float)windowInfo.windowHeight));

	shadowShader.Init("../../Binaries/Shaders/Shadows_CS.cso");

	std::vector<Vertex> vertices =
	{
		{ {-1.0f,  1.0f, 0.0f, 1.0f} , {0.0f, 0.0f}}, // Top left
		{ {1.0f,  1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // Top right
		{ {-1.0f, -1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}  }, // Bottom left
		{ {1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} }, // Bottom right
	};
	std::vector<uint32> indices =
	{
		0, 1, 2, // First triangle
		2, 1, 3  // Second triangle
	};
	uavTexture = UAVTexture::Create(Vector2(1200,720));

	fullQuad = Mesh::Create(vertices, indices);
	std::shared_ptr<Material> material = Material::Create("../../Binaries/Shaders/Full_Screen_Quad_VS.cso", "../../Binaries/Shaders/Full_Screen_Quad_PS.cso");
	fullQuad->SetMaterial(0, material);
	fullQuad->GetMaterial(0)->SetTexture(0, uavTexture->GetTexture2D());

	auto context = Renderer::GetInstance()->GetDeviceContext();

	gChunksData = Texture3D::CreateUAV(Vector3(CHUNK_COUNT * CHUNK_SIZE_XZ, CHUNK_SIZE_Y, CHUNK_COUNT * CHUNK_SIZE_XZ));
	for (int z = 0; z < CHUNK_COUNT; z++)
	{
		for (int x = 0; x < CHUNK_COUNT; x++)
		{
			int chunkIndex = z * CHUNK_COUNT + x;
			int offsetX = x * CHUNK_SIZE_XZ;  
			int offsetZ = z * CHUNK_SIZE_XZ;

			D3D11_BOX region;
			region.left = 0;
			region.right = CHUNK_SIZE_XZ;
			region.top = 0;
			region.bottom = CHUNK_SIZE_Y;
			region.front = 0;
			region.back = CHUNK_SIZE_XZ;
			
			context->CopySubresourceRegion(
				gChunksData->GetTexture3D(),                // Destination texture
				0,                           // Mip level
				offsetX, 0, offsetZ,         // Destination X, Y, Z in large texture
				chunks[chunkIndex].voxelsTexture->GetTexture3D(),   // Source chunk texture
				0,                           // Source subresource index
				&region                      // Source region
			);
		}
	}

	std::vector<ObjectBounds> chunkBoundsData(CHUNK_COUNT * CHUNK_COUNT);
	for (int x = 0; x < CHUNK_COUNT; x++)
	{
		for (int z = 0; z < CHUNK_COUNT; z++)
		{
			int index = z * CHUNK_COUNT + x;
			chunkBoundsData[index].min = { (float)(x * CHUNK_SIZE_XZ), 0, (float)(z * CHUNK_SIZE_XZ) };
			chunkBoundsData[index].max = { (float)((x + 1) * CHUNK_SIZE_XZ), (float)CHUNK_SIZE_Y, (float)((z + 1) * CHUNK_SIZE_XZ) };
		}
	}
	
	gChunkMinMaxBuffer = StructuredBuffer::Create(sizeof(ObjectBounds), (uint32)chunkBoundsData.size(), chunkBoundsData.data());
}

void GameWorld::Render()
{
	colorBuffer.Bind(ConstantBuffers::eMaterialConstantBuffer);
	

	gbuffer->ClearRenderTargets();
	gbuffer->BindRenderTargets();
	
	Renderer::GetInstance()->DisableDepthWriting();
	skySphere->Draw();
	Renderer::GetInstance()->EnableDepthWriting();
	
	for (int i = 0; i < chunks.size(); i++)
	{
		chunks[i].cube->Draw();
	}
	gbuffer->UnbindRenderTargets();

	//gbuffer->BindTexturesToCS(0);
	//uavTexture->BindCS(0);
	//gChunksData->BindCS(3);
	//gChunkMinMaxBuffer->Bind(4);
	//shadowShader.Dispatch((1200 + 15) / 16, (720 + 15) / 16, 1);
	//uavTexture->UnbindCS(0);
	//gbuffer->UnbindTexturesFromCS(0);
	
	Renderer::GetInstance()->RenderToBackbuffer();
	gbuffer->BindTexturesToPS(1);
	fullQuad->Draw();

	if (!GetAsyncKeyState(VK_RBUTTON))
	{
		InputMapper::GetInstance()->ReleaseMouse();
		return;
	}

	Transform& transform = mCamera.GetTransform();
	Matrix matrix = transform.GetMatrix();
	Vector3 position = transform.GetPosition();
	Vector3 right = matrix.Right();
	Vector3 up = matrix.Up();
	Vector3 forward = matrix.Forward();

	float realMovementSpeed = 5 * Timer::GetInstance()->GetDeltaTime();

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		realMovementSpeed = 10 * Timer::GetInstance()->GetDeltaTime();
	}

	if (GetAsyncKeyState('W'))
	{
		position = position + (forward * -realMovementSpeed);
	}
	if (GetAsyncKeyState('S'))
	{
		position = position + (forward * realMovementSpeed);
	}
	if (GetAsyncKeyState('D'))
	{
		position = position + (right * realMovementSpeed);
	}
	if (GetAsyncKeyState('A'))
	{
		position = position + (right * -realMovementSpeed);
	}
	if (GetAsyncKeyState('E'))
	{
		position = position + (up * realMovementSpeed);
	}
	if (GetAsyncKeyState('Q'))
	{
		position = position + (up * -realMovementSpeed);
	}

	Vector3 currentRotation = transform.GetEularRotation();
	Vector3 targetRotation = currentRotation;
	Vector2 dPos = InputMapper::GetInstance()->GetMouseDelta();
	if (dPos.x != 0 || dPos.y != 0)
	{
		float realRotationSpeed = 1 * Timer::GetInstance()->GetDeltaTime();

		targetRotation.y += realRotationSpeed * dPos.x;
		targetRotation.x += realRotationSpeed * dPos.y;

		constexpr float maxPitchAngle = 3.14159265359f / 2.0f - 0.01f;
		targetRotation.x = CLAMP(targetRotation.x, -maxPitchAngle, maxPitchAngle);
	}

	float lerpSpeed = 30.0f;
	Vector3 smoothedRotation = {};
	smoothedRotation.x = std::lerp(currentRotation.x, targetRotation.x, lerpSpeed * Timer::GetInstance()->GetDeltaTime());
	smoothedRotation.y = std::lerp(currentRotation.y, targetRotation.y, lerpSpeed * Timer::GetInstance()->GetDeltaTime());
	smoothedRotation.z = 0.0f;
	transform.SetRotation(smoothedRotation);

	transform.SetPosition(position);
	InputMapper::GetInstance()->CaptureMouse();
}

void InitGameWorld()
{
	gameworld.Init();
}

void RenderGameWorld()
{
	gameworld.Render();
}
