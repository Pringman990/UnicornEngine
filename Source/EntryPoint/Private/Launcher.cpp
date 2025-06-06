#include <stdint.h>
#include <EngineSubsystem.h>
#include "EngineLoop.h"
#include <Core.h>

#include <EngineComponents/ETransform.h>

int32_t GuardedMain()
{
	_TRACK_MEMORY(true, true);
	{
		FileSystem::Init();

		Logger::Create();
		Logger::Get()->Init();

		FileWatcherSubsystem::Create();
		FileWatcherSubsystem::Get()->Init(std::filesystem::current_path().parent_path().parent_path().string(), FileWatcherBackendFactory::Create());

		//AssetRegistry::Create();
		ThreadPool::Create();
		ThreadPool* threadPool = ThreadPool::Get();
		EngineLoop engineLoop;

		Timer::Create();
		Timer* timer = Timer::Get();

		ReflectionRegistry::Create();

		ModuleManager::Create();

		AssetManager::Create();

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
	
	AssetManager::Shutdown();
	ModuleManager::Shutdown();
	Timer::Shutdown();
	ThreadPool::Shutdown();
	FileWatcherSubsystem::Shutdown();
	Logger::Shutdown();


	__SingletonRegistryFunctions::EnsureAllShutdown();
	_STOP_TRACK_MEMORY();

	return 0;
}