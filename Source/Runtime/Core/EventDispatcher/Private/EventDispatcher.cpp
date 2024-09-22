#include "EventDispatcher.h"

EventDispatcher* EventDispatcher::mInstance = nullptr;

EventDispatcher::EventDispatcher()
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::ShutDown()
{
	delete mInstance;
	mInstance = nullptr;
}

void EventDispatcher::Dispatch(DispatchEvents aEvent)
{
	auto it = mEvents.Find(aEvent);
	if (it.IsValid())
	{
		auto& receivers = it.Value();
		for (uint32_t i = 0; i < mEvents.Size(); i++)
		{
			receivers[i]();
		}
	}
}

void EventDispatcher::RegisterReceiver(DispatchEvents aEvent, TFunction<void> aCallback)
{
	mEvents[aEvent].AddUnique(aCallback);
}
