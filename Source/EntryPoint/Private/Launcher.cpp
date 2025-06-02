#include <stdint.h>
#include <Singleton.h>
#include "EngineLoop.h"
#include <Core.h>

#include <EngineComponents/ETransform.h>

int32_t GuardedMain()
{
	_TRACK_MEMORY(true, true);
	{
		FileSystem::Init();

		Logger::Create();
		Logger::GetInstance()->Init();

		FileWatcherSubsystem::Create();
		FileWatcherSubsystem::GetInstance()->Init(std::filesystem::current_path().parent_path().parent_path().string(), FileWatcherBackendFactory::Create());

		//AssetRegistry::Create();
		ThreadPool::Create();
		ThreadPool* threadPool = ThreadPool::GetInstance();
		EngineLoop engineLoop;

		Timer::Create();
		Timer* timer = Timer::GetInstance();

		ReflectionRegistry::Create();

		ModuleManager::Create();

		if (!engineLoop.Init())
		{
			_ENSURE_CORE(false, "Engine Loop Failed To Init");
			return 0;
		}

		_LOG_CORE_INFO("GuardedMain has Initilized and will now run the main loop");
		//This is the main loop for the whole engine
		while (engineLoop.EngineLoopClose())
		{
			threadPool->ProcessCallbacks();
			timer->Update();
			engineLoop.Update();
			threadPool->ProcessCallbacks();
		}

		_LOG_CORE_INFO("Main loop exited, starting cleanup");
		_LOG_CORE_INFO("Main cleanup done, Goodbye welcome back!");

		ReflectionRegistry::Shutdown();
	}
	
	ModuleManager::Shutdown();
	Timer::Shutdown();
	ThreadPool::Shutdown();
	Logger::Shutdown();

	//AssetRegistry::Shutdown();

	FileWatcherSubsystem::Shutdown();

	_Singleton::CheckAllHasShutdown();
	_STOP_TRACK_MEMORY();

	return 0;
}