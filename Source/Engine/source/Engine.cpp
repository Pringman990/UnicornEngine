#include "EnginePch.h"
#include "Engine.h"

#include <WinUser.h>

#include "input/InputManager.h"
#include <source/debug/MemoryTracker.h>
#include "scene/SceneManager.h"
#include "serialization/SceneSerializer.h"

//Systems
#include "ecs/systems/MeshRenderSystem.h"
#include "ecs/systems/SpriteRenderSystem.h"

//Components
#include "ecs/components/Transform.h"
#include "ecs/components/Mesh.h"
#include "ecs/components/Sprite.h"

Engine* Engine::mInstance = nullptr;

Engine::Engine()
	:
	mShouldClose(false),
	mGraphicsEngine(new GraphicsEngine(), &Engine::ShutdownGrapicsEngine),
	mECSSystemManager(new ecs::SystemManager(), &Engine::ShutdownSystemManager),
	mSceneManager(new SceneManager, &Engine::ShutdownSceneManager),
	mSceneSerializer(new SceneSerializer, &Engine::ShutdownSceneSerializer)
{
}

Engine::~Engine()
{
}

Engine& Engine::GetInstance()
{
	return *mInstance;
}

bool Engine::Start()
{
	if (mInstance == nullptr)
	{
		StartMemoryTracking({ false, false });

		mInstance = new Engine();
		return mInstance->Init();
	}

	return false;
}

void Engine::ShutDown()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;

		StopMemoryTrackingAndPrint();
	}
}

GraphicsEngine& Engine::GetGraphicsEngine()
{
	if (mInstance)
		return *mInstance->mGraphicsEngine;
	else
		return *GetInstance().mGraphicsEngine;
}

ecs::SystemManager& Engine::GetECSSystemManager()
{
	if (mInstance)
		return *mInstance->mECSSystemManager;
	else
		return *GetInstance().mECSSystemManager;
}

SceneManager& Engine::GetSceneManager()
{
	if (mInstance)
		return *mInstance->mSceneManager;
	else
		return *GetInstance().mSceneManager;
}

SceneSerializer& Engine::GetSceneSerializer()
{
	if (mInstance)
		return *mInstance->mSceneSerializer;
	else
		return *GetInstance().mSceneSerializer;
}

void Engine::InitEngineSystems()
{
	{
		auto meshSystem = [](ecs::World& world, ecs::Entity entity, ecs::Transform& transform, ecs::Mesh& mesh) {
			ecs::engineSystem::MeshRenderSystem(world, entity, transform, mesh);
			};

		mECSSystemManager->_RegisterEngineSystem<ecs::Transform, ecs::Mesh>(meshSystem, "Mesh Render System", ecs::OnRender);
	}

	{
		auto spriteSystem = [](ecs::World& world, ecs::Entity entity, ecs::Transform& transform, ecs::Sprite& sprite) {
			ecs::engineSystem::SpriteRenderSystem(world, entity, transform, sprite);
			};

		mECSSystemManager->_RegisterEngineSystem<ecs::Transform, ecs::Sprite>(spriteSystem, "Sprite Render System", ecs::OnRender);
	}
}

bool Engine::Init()
{
	if (!mGraphicsEngine->Init())
		return false;

	if (!Input::Init())
		return false;

	InitEngineSystems();

	return true;
}

void Engine::ShutdownGrapicsEngine(GraphicsEngine* aGraphicsEngine)
{
	delete aGraphicsEngine;
	aGraphicsEngine = nullptr;
}

void Engine::ShutdownSystemManager(ecs::SystemManager* aSystemManager)
{
	delete aSystemManager;
	aSystemManager = nullptr;
}

void Engine::ShutdownSceneManager(SceneManager* aSceneManager)
{
	delete aSceneManager;
	aSceneManager = nullptr;
}

void Engine::ShutdownSceneSerializer(SceneSerializer* aSceneSerializer)
{
	delete aSceneSerializer;
	aSceneSerializer = nullptr;
}

bool Engine::BeginFrame()
{
	if (mShouldClose)
		return false;

	MSG msg = {};
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			mShouldClose = true;
		}
	}

	mTimer.Update();

	Input::Update();

	mGraphicsEngine->PreRender();

	return true;
}

void Engine::Update(bool RunGame)
{
	mECSSystemManager->RunUpdateEngineSystems();

	if(RunGame)
		mSceneManager->GetCurrentEcsWorld().ProcessUpdateSystems();
}

void Engine::Render(bool /*RunGame*/)
{
	mECSSystemManager->RunRenderEngineSystems();
}

void Engine::EndFrame()
{
	mGraphicsEngine->PostRender();
}

void Engine::SetShouldClose(bool shouldClose)
{
	mShouldClose = shouldClose;
}
