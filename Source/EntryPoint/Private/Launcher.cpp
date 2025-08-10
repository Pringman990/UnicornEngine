#include <stdint.h>
#include <EngineSubsystem.h>
#include "EngineLoop.h"
#include <Core.h>

int32_t GuardedMain()
{
	_TRACK_MEMORY(true, true);
	{
		FileSystem::Create();
		FileSystem::Get()->Init();

		Logger::Create();
		Logger::Get()->Init();

		ReflectionRegistry::Create();

		FileWatcherSubsystem::Create();
		FileWatcherSubsystem::Get()->Init(std::filesystem::current_path().parent_path().parent_path().string(), FileWatcherBackendFactory::Create());

		Timer::Create();
		Timer* timer = Timer::Get();

		ModuleManager::Create();

		EngineLoop engineLoop;
		_ASSERT_CORE(engineLoop.Init(), "Engine Loop Failed To Init");

		_LOG_CORE_INFO("GuardedMain has Initilized and will now run the main loop");
		//This is the main loop for the whole engine
		while (engineLoop.EngineLoopClose())
		{
			timer->Update();
			engineLoop.Update();
		}

		_LOG_CORE_INFO("Main loop exited, starting cleanup");
	}
	
	ModuleManager::Shutdown();
	Timer::Shutdown();
	FileWatcherSubsystem::Shutdown();

	ReflectionRegistry::Shutdown();

	_LOG_CORE_INFO("Main cleanup done, Goodbye welcome back!");
	Logger::Shutdown();

	FileSystem::Shutdown();

	__SingletonRegistryFunctions::EnsureAllShutdown();
	_STOP_TRACK_MEMORY();

	return 0;
}