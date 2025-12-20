#include "Timer/Timer.h"

Timer* Timer::sInstance = nullptr;
REGISTER_ENGINE_SUBSYSTEM(Timer)

ENGINE_API Timer* Timer::Instance()
{
#ifdef _DEBUG
	ASSERT(sInstance, "Instance was accessed before/after it was created/destroyed");
#endif
	return sInstance;
}