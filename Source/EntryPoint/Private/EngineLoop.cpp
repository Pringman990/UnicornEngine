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

	ModuleManager::GetInstance()->UnLoadModule("Sandbox");

	InputMapper::Shutdown();
	Renderer::Shutdown();
	FileWatcher::Shutdown();
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
	mGenericApplication = Application::GetInstance()->_CreateApplication();

	mGenericApplication->OnApplicationRequestExist.AddRaw(this, &EngineLoop::RequestExit);

	if (!mGenericApplication->Init())
	{
		_ENSURE_CORE(false, "Engine Loop Failed To Create Application");
		return false;
	}

	{
		FileWatcher::Create();
		if (!FileWatcher::GetInstance()->Init())
		{
			_ENSURE_CORE(false, "Engine Loop Failed To Init FileWatcher");
			return false;
		}
		mFileWatcher = FileWatcher::GetInstance();
	}

	InputMapper::Create();
	InputMapper::GetInstance()->Init();

	{
		_LOG_CORE_INFO("Creating Renderer");
		_PAUSE_TRACK_MEMORY(true); // We turn off tracking because there is a phantom memory leak
		
		Renderer::Create();
		
		_PAUSE_TRACK_MEMORY(false);
		
		mRenderer = Renderer::GetInstance();
		_ENSURE_CORE(mRenderer, "Engine Loop Failed To Create Renderer");

		_LOG_CORE_INFO("Renderer Initializing");
		
		TIMER_START_READING("__Engine Loop Renderer Init__");
		if (!mRenderer->Init())
		{
			_ENSURE_CORE(false, "Engine Loop Failed To Init Renderer");
			return false;
		}
		
		float rendererInitTime = TIMER_END_READING("__Engine Loop Renderer Init__");
		_LOG_CORE_INFO("Renderer has finished Initialize, it took: {:0.7f}s", rendererInitTime);
	}

#ifdef _EDITOR
	{
		_LOG_CORE_INFO("Editor Initializing");
		TIMER_START_READING("__Engine Loop Editor Init__");

		mEditor = new Editor();
		if (!mEditor->Init())
		{
			_ENSURE_CORE(false, "Engine Loop Failed To Init Editor");
			return false;
		}

		float editorInitTime = TIMER_END_READING("__Engine Loop Editor Init__");
		_LOG_CORE_INFO("Editor has finished Initialize, it took: {:0.7f}s", editorInitTime);
	}
#endif // _EDITOR

	{
		_PAUSE_TRACK_MEMORY(true);

		if (!ModuleManager::GetInstance()->LoadModule("Sandbox"))
		{
			return false;
		}

		HMODULE sanboxModule = ModuleManager::GetInstance()->GetHModule("Sandbox");

		SandboxInit initGameWorld = (SandboxInit)GetProcAddress(sanboxModule, "InitGameWorld");
		if (!initGameWorld) {
			std::cerr << "Could not init gameworld" << std::endl;
		}
		initGameWorld();

		mSandboxRender = (SandboxRender)GetProcAddress(sanboxModule, "RenderGameWorld");
		if (!mSandboxRender) {
			std::cerr << "Could not load render gameworld" << std::endl;
			return false;
		}

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

	mFileWatcher->Watch();

	mRenderer->PreRender();

	mSandboxRender();

	mRenderer->RenderToBackbuffer();
#ifdef _EDITOR
	mEditor->BeginFrame();
	mEditor->Render();
#endif // _EDITOR

#ifdef _EDITOR
	mEditor->EndFrame();
#endif // _EDITOR

	mRenderer->Present();
	InputMapper::GetInstance()->Update();
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
