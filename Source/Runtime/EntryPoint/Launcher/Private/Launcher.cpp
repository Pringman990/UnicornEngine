#include <stdint.h>
#include <Logger.h>
#include <Timer.h>
#include <EngineLoop.h>

int32_t GuardedMain()
{
	{
		EngineLoop engineLoop;

		Timer& timer = Timer::GetInstance();

		if (!engineLoop.Init())
		{
			LOG_CORE_INFO("Engine Loop Failed To Init");
			assert(false);
			return 0;
		}

		LOG_CORE_INFO("GuardedMain has Initilized and will now run the main loop");
		//This is the main loop for the whole engine
		while (engineLoop.EngineLoopClose())
		{
			timer.Update();
			engineLoop.Update();
		}

		timer.ShutDown();

		LOG_CORE_INFO("Main loop exited, starting cleanup");

		LOG_CORE_INFO("Main cleanup done, Goodbye welcome back!");
	}
	return 0;
}