#include "EngineLoop.h"

#include <EngineMinimal.h>

#ifdef _EDITOR
#include <Editor.h>
#endif // EDITOR

#include <Input/InputMapper.h>

#include <ESystemManager.h>
#include <EComponentRegistry.h>
#include <EWorld.h>

#include <Module/ModuleManager.h>

EngineLoop::EngineLoop()
	:
	mGenericApplication(nullptr),
	mFileWatcher(nullptr),
	mShouldExit(false)
{
}

EngineLoop::~EngineLoop()
{
#ifdef _EDITOR
	delete mEditor;
	mEditor = nullptr;
#endif // _EDITOR

	SubsystemManager::Get<ESystemManager>()->UnRegisterSystems();
	SubsystemManager::Get<ModuleManager>()->UnLoadModule("Sandbox");

	mGenericApplication = nullptr;
	mFileWatcher = nullptr;
}

bool EngineLoop::Init()
{
	TIMER_START_READING("__Engine Loop Init__");
	LOG_INFO("Engine Loop Starting Init");

	Application* app = SubsystemManager::Get<Application>();
	mGenericApplication = app->_CreateApplication();
	ASSERT(mGenericApplication, "Engine Loop Failed To Create Application");

	ASSERT(mGenericApplication->Init(), "Engine Loop Failed To Init Application");

	mGenericApplication->OnApplicationRequestExist.AddRaw(this, &EngineLoop::RequestExit);

	SubsystemManager::Get<InputMapper>()->Init();

	{
		LOG_INFO("Render Loop Initializing");

		TIMER_START_READING("__Engine Loop Render Loop Init__");
		ASSERT(mRenderLoop.Init(), "Engine Loop Failed To Init Render Loop");

		float rendererInitTime = TIMER_END_READING("__Engine Loop Render Loop Init__");
		LOG_INFO("Render Loop has finished Initialize, it took: {:0.7f}s", rendererInitTime);
	}

#ifdef _EDITOR
	{
		LOG_INFO("Editor Initializing");
		TIMER_START_READING("__Engine Loop Editor Init__");

		mEditor = new Editor();
		ASSERT(mEditor->Init(), "Engine Loop Failed To Init Editor");

		float editorInitTime = TIMER_END_READING("__Engine Loop Editor Init__");
		LOG_INFO("Editor has finished Initialize, it took: {:0.7f}s", editorInitTime);
	}
#endif // _EDITOR

	{
		_PAUSE_TRACK_MEMORY(true);
		ModuleManager* moduleManager = SubsystemManager::Get<ModuleManager>();
		ASSERT(moduleManager->LoadModule("Sandbox"), "Failed to load Sandbox module");

		HMODULE sanboxModule = moduleManager->GetHModule("Sandbox");

		SandboxInit initGameWorld = (SandboxInit)GetProcAddress(sanboxModule, "InitGameWorld");
		ASSERT(initGameWorld, "Could not init gameworld");
		initGameWorld();

		mSandboxRender = (SandboxRender)GetProcAddress(sanboxModule, "RenderGameWorld");
		ASSERT(mSandboxRender, "Could not load render gameworld");

		_PAUSE_TRACK_MEMORY(false);
	}

	auto infos = SubsystemManager::Get<ReflectionRegistry>()->GetAllInfos();
	for (auto& info : infos)
	{
		REFLECTION_LOG_TYPE_INFO(info)
	}

	mWorld = new EWorld();
	SubsystemManager::Get<ESystemManager>()->RunLoad(*mWorld);

	float initTime = TIMER_END_READING("__Engine Loop Init__");
	LOG_INFO("Engine Loop has finished Initialize, it took: {:0.7f}s", initTime);
	return true;
}

void EngineLoop::Update()
{
	mGenericApplication->Update();
	if (mShouldExit)
		return;

	SubsystemManager::Get<InputMapper>()->Update();

	mRenderLoop.BeginFrame();
	
	SubsystemManager::Get<ESystemManager>()->RunUpdate(*mWorld);
	//mSandboxRender();

	//mRenderer->SwitchToSwapChain();
	mRenderLoop.Execute();
	{
#ifdef _EDITOR
		//RenderScope scope(mRenderer->GetCurrentSwapChainTexture());
		mEditor->BeginFrame();
		mEditor->Render();
		mEditor->EndFrame();
#endif // _EDITOR

#ifdef _EDITOR
#endif // _EDITOR
	}

	mRenderLoop.EndFrame();
}

void EngineLoop::RequestExit()
{
	LOG_INFO("Engine Loop exit has been requested");
	mShouldExit = true;
}

bool EngineLoop::EngineLoopClose() const
{
	return !mShouldExit;
}
