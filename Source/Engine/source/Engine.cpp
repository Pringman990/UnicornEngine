#include "EnginePch.h"
#include "Engine.h"

#include "input/InputManager.h"

#include <WinUser.h>
#include <source/debug/MemoryTracker.h>

Engine* Engine::mInstance = nullptr;

Engine::Engine()
	:
	mShouldClose(false)
{
}

Engine::~Engine()
{
}

Engine& Engine::GetEngine()
{
	return *mInstance;
}

bool Engine::Start()
{
	if (mInstance == nullptr)
	{
		StartMemoryTracking({ true, true });

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

bool Engine::Init()
{
	if (!Input::Init())
		return false;
	
	return true;
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

	GraphicsEngine::GetInstance().PreRender();

    return true;
}

void Engine::Update()
{
	
}

void Engine::Render()
{
	GraphicsEngine::GetInstance().Render();
}

void Engine::EndFrame()
{
	GraphicsEngine::GetInstance().PostRender();
}

void Engine::SetShouldClose(bool shouldClose)
{
	mShouldClose = shouldClose;
}
