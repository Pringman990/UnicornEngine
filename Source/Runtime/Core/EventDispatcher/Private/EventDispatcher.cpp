#include "EventDispatcher.h"

EventDispatcher* EventDispatcher::_sInstance = nullptr;

EventDispatcher::EventDispatcher()
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::ShutDown()
{
	delete _sInstance;
	_sInstance = nullptr;
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
