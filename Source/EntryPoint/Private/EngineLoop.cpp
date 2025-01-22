#include "EngineLoop.h"

#include <Application/Application.h>
#include <Application/Generic/GenericApplication.h>
#include <Renderer.h>
#include <RendererFactory.h>

#include <EventDispatcher/EventDispatcher.h>
#include <AssetRegistry.h>
#include <Input/InputMapper.h>

#include <Logger/Logger.h>
#include <Timer/Timer.h>
#include <SimpleMath.h>
#include <FileWatcher/FileWatcher.h>

#ifdef _EDITOR
#include <Editor.h>
#endif // EDITOR

EngineLoop::EngineLoop()
	:
	mShouldExit(false),
	mGenericApplication(nullptr),
	mRenderer(nullptr),
	mSandboxRender(nullptr),
	mSandBoxModule()

{
}

EngineLoop::~EngineLoop()
{
#ifdef _EDITOR
	delete mEditor;
	mEditor = nullptr;
#endif // _EDITOR

	Renderer::Shutdown();
	InputMapper::Shutdown();
	FileWatcher::Shutdown();
	Application::Shutdown();
	EventDispatcher::Shutdown();

	mRenderer = nullptr;
	mGenericApplication = nullptr;

	FreeLibrary(mSandBoxModule);
}

bool EngineLoop::Init()
{
	TIMER_START_READING("__Engine Loop Init__");
	_LOG_CORE_INFO("Engine Loop Starting Init");

	EventDispatcher::Create();
	EventDispatcher::GetInstance()->RegisterReceiver(DispatchEvents::eEngineExit, [&]() { RequestExit(); });

	Application::Create();
	mGenericApplication = Application::GetInstance()->_CreateApplication();

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
		mRenderer = RendererFactory::CreateRenderer();
		_ENSURE_CORE(mRenderer, "Engine Loop Failed To Create Renderer");

		_LOG_CORE_INFO("Renderer Initilizing");
		TIMER_START_READING("__Engine Loop Renderer Init__");
		if (!mRenderer->Init())
		{
			_ENSURE_CORE(false, "Engine Loop Failed To Init Renderer");
			return false;
		}
		float rendererInitTime = TIMER_END_READING("__Engine Loop Renderer Init__");
		_LOG_CORE_INFO("Renderer has finished Initilize, it took: {:0.7f}s", rendererInitTime);
	}

#ifdef _EDITOR
	{
		mEditor = new Editor();
		if (!mEditor->Init())
		{
			_ENSURE_CORE(false, "Engine Loop Failed To Init Editor");
			return false;
		}
	}
#endif // _EDITOR

	{
		_PAUSE_TRACK_MEMORY(true);
#ifdef _WIN64

#ifdef _EDITOR
#ifdef _MEMORY_DEBUG
		mSandBoxModule = LoadLibraryA("Sandbox_MemoryDebug Editor.dll");
		if (!mSandBoxModule)
		{
			return false;
		}
#else
		mSandBoxModule = LoadLibraryA("Sandbox_Debug editor.dll");
		if (!mSandBoxModule)
		{
			return false;
		}
#endif // _MEMORY_DEBUG
#else
#ifdef _MEMORY_DEBUG
		mSandBoxModule = LoadLibraryA("Sandbox_MemoryDebug.dll");
		if (!mSandBoxModule)
		{
			return false;
		}
#else
		mSandBoxModule = LoadLibraryA("Sandbox_Debug.dll");
		if (!mSandBoxModule)
		{
			return false;
		}
#endif
#endif // _EDITOR

#endif // WIN64

		SandboxInit initGameWorld = (SandboxInit)GetProcAddress(mSandBoxModule, "InitGameWorld");

		if (!initGameWorld) {
			std::cerr << "Could not locate the functions" << std::endl;
			FreeLibrary(mSandBoxModule);
			return false;
		}

		initGameWorld();

		mSandboxRender = (SandboxRender)GetProcAddress(mSandBoxModule, "RenderGameWorld");

		if (!mSandboxRender) {
			std::cerr << "Could not locate the functions" << std::endl;
			FreeLibrary(mSandBoxModule);
			return false;
		}

		_PAUSE_TRACK_MEMORY(false);
	}

	float initTime = TIMER_END_READING("__Engine Loop Init__");
	_LOG_CORE_INFO("Engine Loop has finished Initilize, it took: {:0.7f}s", initTime);
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

#ifdef _EDITOR
	mEditor->BeginFrame();
	mEditor->Render();
#endif // _EDITOR

	mRenderer->GetRenderQueue()->Execute();

	mRenderer->RenderToBackbuffer();

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
