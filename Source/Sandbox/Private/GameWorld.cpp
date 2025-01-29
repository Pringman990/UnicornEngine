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
#include "ChunkMesher.h"

#include <ConstantBuffer/CSConstantBuffer.h>
#include <ConstantBuffer/CommonConstantBuffers.h>
#include <ComputeShader.h>
#include <Voxel.h>
#include <Texture.h>
#include <Mesh.h>
#include <Threading/ThreadPool.h>


namespace {
	GameWorld gameworld;
	ComputeShader mainRayShader;
	CSConstantBuffer cscBuffer;
	Texture2D* testTexture;
	std::shared_ptr<Mesh> quad;
	std::shared_ptr<Mesh> cube;
	std::vector<Chunk> chunks;
	std::vector<std::shared_ptr<Mesh>> testMesh;
	std::mutex chunksMutex;
}

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}

void GenerateAndStoreChunk(int Count) {
	std::vector<Chunk> chunks2 = ChunkGenerator::GenerateChunksFromPerlin(Count);

	// Lock and store the chunk safely
	std::lock_guard<std::mutex> lock(chunksMutex);
	for (size_t i = 0; i < chunks2.size(); i++)
	{
		chunks2[i].RemoveOccludedVoxels();
		testMesh.push_back(ChunkMesher::MeshifyChunk(chunks2[i]));
	}
}

void GameWorld::Init()
{
	LOG_INFO("GameWorld init...");

	float aspect = (16.f / 9.f);
	mCamera.SetPerspective(90, aspect, 0.001f, 1000.f);
	mCamera.GetTransform().SetPosition({ 0,0,-5 });
	Renderer::GetInstance()->SetMainCamera(&mCamera);

	ThreadPool::GetInstance()->Enqueue([](){ GenerateAndStoreChunk(10); });

//	chunks = ChunkGenerator::GenerateChunksFromPerlin(20);
	//chunks[0].RemoveOccludedVoxels();
	//mainRayShader.Init(
	//	"../../Binaries/Shaders/PrimaryRay_CS.cso", 
	//	sizeof(Voxel), 
	//	(uint32)chunks.size() * (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE),
	//	1200,
	//	720,
	//	&chunks[0].voxels
	//);
	//for (int i = 0; i < chunks.size(); i++)
	//{
	//	//chunks[i].RemoveOccludedVoxels();
	//	testMesh.push_back(ChunkMesher::MeshifyChunk(chunks[i]));
	//}

	std::vector<Vertex> vertices =
	{
		{ {-1.0f,  1.0f, 0.0f, 1.0f}, {0.0f, 0.0f} }, // Top left
		{ {1.0f,  1.0f, 0.0f, 1.0f}, {1.0f, 0.0f} }, // Top right
		{ {-1.0f, -1.0f, 0.0f, 1.0f}, {0.0f, 1.0f} }, // Bottom left
		{ {1.0f, -1.0f, 0.0f, 1.0f}, {1.0f, 1.0f} }, // Bottom right
	};
	std::vector<uint32> indices =
	{
		0, 1, 2, // First triangle
		2, 1, 3  // Second triangle
	};
	quad = Mesh::Create(vertices, indices, "../../Binaries/Shaders/Full_Screen_Quad_VS.cso", "../../Binaries/Shaders/Full_Screen_Quad_PS.cso");

	//cscBuffer.Init(sizeof(CameraConstantBufferData));

	std::vector<Vertex> vertices2 =
	{

		{{ - 0.5f, 0.5f, -0.5f, 1},  {1.0f, 0.0f} },

		{{ 0.5f, 0.5f, -0.5f, 1}, {0.0f, 1.0f}},

		{ {-0.5f, -0.5f, -0.5f, 1 }, {0.0f, 0.0f} },
		
		{ { 0.5f, -0.5f, -0.5f , 1 }, {1.0f, 0.0f}},
		
		{ { -0.5f, 0.5f, 0.5f , 1 }, {0.0f, 1.0f}},
		
		{ { 0.5f, 0.5f, 0.5f , 1 }, {1.0f, 0.0f} },
		
		{ { -0.5f, -0.5f, 0.5f , 1 }, {1.0f, 1.0f} },
		
		{ {0.5f, -0.5f, 0.5f , 1 }, {1.0f, 1.0f}},

	};

	std::vector<uint32> indices2 =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
	};
	cube = Mesh::Create(vertices2, indices2, "../../Binaries/Shaders/Lit_Model_VS.cso", "../../Binaries/Shaders/Lit_Model_PS.cso");
	cube->GetMatrix().Translation(Vector3(-1,0,0));
}

void GameWorld::Render()
{
	//static Camera equals;
	//if (equals != mCamera)
	//{
	//	CameraConstantBufferData data; 
	//	data.cameraPosition = mCamera.GetTransform().GetPosition();
	//	data.worldToClipMatrix = mCamera.GetClipSpaceMatrix().Invert();
	//	cscBuffer.UpdateData(&data);
	//	cscBuffer.Bind();
	//	equals = mCamera;
	//}

	//mainRayShader.BindAndDispatch((1200 + 15) / 16, (720+15) / 16, 1);
	
	//mainRayShader.GetTexture()->Bind(0);
	//quad->Draw();
	for (int i = 0; i < testMesh.size(); i++)
	{
		Renderer::GetInstance()->UpdateObjectBuffer(testMesh[i]->GetMatrix());
		testMesh[i]->Draw();
	}
	//Renderer::GetInstance()->UpdateObjectBuffer(cube->GetMatrix());
	//cube->Draw();

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
