#include <stdint.h>
#include <Logger.h>
#include <Timer.h>
#include "EngineLoop.h"
#include <MemoryDebugger.h>
#include <ReflectionRegistry.h>

#include <ResourceRegistry.h>

int32_t GuardedMain()
{
	_TRACK_MEMORY(true, true);
	{
		ResourceRegistry::Create();
		Logger::Create();
		Logger::GetInstance()->Init();

		EngineLoop engineLoop;

		Timer::Create();
		Timer* timer = Timer::GetInstance();

		if (!engineLoop.Init())
		{
			_ENSURE_CORE(false, "Engine Loop Failed To Init");
			return 0;
		}

		_LOG_CORE_INFO("GuardedMain has Initilized and will now run the main loop");
		//This is the main loop for the whole engine
		while (engineLoop.EngineLoopClose())
		{
			timer->Update();
			engineLoop.Update();
		}

		_LOG_CORE_INFO("Main loop exited, starting cleanup");
		_LOG_CORE_INFO("Main cleanup done, Goodbye welcome back!");

		ReflectionRegistry::Shutdown();
		Timer::Shutdown();
		Logger::Shutdown();
	}
	ResourceRegistry::Shutdown();
	_STOP_TRACK_MEMORY();

	return 0;
}