#include "EngineLoop.h"
#include <Renderer.h>

#ifdef _EDITOR
#include <Editor.h>
#endif // EDITOR

EngineLoop::EngineLoop()
	:
	mGenericApplication(nullptr),
	mRenderer(nullptr),
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

	ModuleManager::Get()->UnLoadModule("Sandbox");

	//InputMapper::Shutdown();
	Renderer::Shutdown();
	Application::Shutdown();
	
	mRenderer = nullptr;
	mGenericApplication = nullptr;
	mFileWatcher = nullptr;
}

bool EngineLoop::Init()
{
	TIMER_START_READING("__Engine Loop Init__");
	_LOG_CORE_INFO("Engine Loop Starting Init");

	Application::Create();
	mGenericApplication = Application::Get()->_CreateApplication();
	_ASSERT_CORE(mGenericApplication, "Engine Loop Failed To Create Application");

	_ASSERT_CORE(mGenericApplication->Init(), "Engine Loop Failed To Init Application");
	
	mGenericApplication->OnApplicationRequestExist.AddRaw(this, &EngineLoop::RequestExit);

	//InputMapper::Create();
	//InputMapper::Get()->Init();

	{
		_LOG_CORE_INFO("Creating Renderer");
		_PAUSE_TRACK_MEMORY(true); // We turn off tracking because there is a phantom memory leak
		
		Renderer::Create();
		
		_PAUSE_TRACK_MEMORY(false);
		
		mRenderer = Renderer::Get();
		_ASSERT_CORE(mRenderer, "Engine Loop Failed To Create Renderer");
	
		_LOG_CORE_INFO("Renderer Initializing");
		
		TIMER_START_READING("__Engine Loop Renderer Init__");
		_ASSERT_CORE(mRenderer->Init(), "Engine Loop Failed To Init Renderer");
		
		float rendererInitTime = TIMER_END_READING("__Engine Loop Renderer Init__");
		_LOG_CORE_INFO("Renderer has finished Initialize, it took: {:0.7f}s", rendererInitTime);
	}

#ifdef _EDITOR
	{
		_LOG_CORE_INFO("Editor Initializing");
		TIMER_START_READING("__Engine Loop Editor Init__");

		mEditor = new Editor();
		_ASSERT_CORE(mEditor->Init(), "Engine Loop Failed To Init Editor");

		float editorInitTime = TIMER_END_READING("__Engine Loop Editor Init__");
		_LOG_CORE_INFO("Editor has finished Initialize, it took: {:0.7f}s", editorInitTime);
	}
#endif // _EDITOR

	{
		_PAUSE_TRACK_MEMORY(true);

		_ASSERT_CORE(ModuleManager::Get()->LoadModule("Sandbox"), "Failed to load Sandbox module");

		HMODULE sanboxModule = ModuleManager::Get()->GetHModule("Sandbox");

		SandboxInit initGameWorld = (SandboxInit)GetProcAddress(sanboxModule, "InitGameWorld");
		_ASSERT_CORE(initGameWorld, "Could not init gameworld");
		initGameWorld();

		mSandboxRender = (SandboxRender)GetProcAddress(sanboxModule, "RenderGameWorld");
		_ASSERT_CORE(mSandboxRender, "Could not load render gameworld");

		_PAUSE_TRACK_MEMORY(false);
	}

	ReflectionRegistry::ProcessDefferedRegistration();

	float initTime = TIMER_END_READING("__Engine Loop Init__");
	_LOG_CORE_INFO("Engine Loop has finished Initialize, it took: {:0.7f}s", initTime);
	return true;
}

void EngineLoop::Update()
{
	mGenericApplication->Update();
	if (mShouldExit)
		return;

	mRenderer->PreRender();

	mSandboxRender();

	//mRenderer->RenderToBackbuffer();
#ifdef _EDITOR
	mEditor->BeginFrame();
	mEditor->Render();
#endif // _EDITOR

#ifdef _EDITOR
	mEditor->EndFrame();
#endif // _EDITOR

	//mRenderer->Present();
	//InputMapper::Get()->Update();
}

void EngineLoop::RequestExit()
{
	_LOG_CORE_INFO("Engine Loop exit has been requested");
	mShouldExit = true;
}

bool EngineLoop::EngineLoopClose() const
{
	return !mShouldExit;
}
