#include <stdint.h>
#include "EngineLoop.h"

#include <EngineMinimal.h>

int32_t GuardedMain()
{
	Logger::Init();

	_TRACK_MEMORY(true, true);
	{
		SubsystemManager::Init();
		
		SubsystemManager::Get<FileSystem>()->Init();

		SubsystemManager::Get<FileWatcherSubsystem>()->Init(std::filesystem::current_path().parent_path().parent_path().string(), FileWatcherBackendFactory::Create());

		Timer* timer = SubsystemManager::Get<Timer>();

		EngineLoop engineLoop;
		ASSERT(engineLoop.Init(), "Engine Loop Failed To Init");

		LOG_INFO("GuardedMain has Initilized and will now run the main loop");
		//This is the main loop for the whole engine
		while (engineLoop.EngineLoopClose())
		{
			timer->Update();
			engineLoop.Update();
		}

		LOG_INFO("Main loop exited, starting cleanup");
	}

	LOG_INFO("Main cleanup done, Goodbye welcome back!");
	
	SubsystemManager::DestroyAll();
	__Subsystem::EnsureAllShutdown();
	_STOP_TRACK_MEMORY();
	Logger::Cleanup();

	return 0;
}