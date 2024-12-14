#include "pch.h"
#include "EventDispatcher.h"

EventDispatcher::EventDispatcher()
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::Dispatch(DispatchEvents aEvent)
{
	auto it = mEvents.find(aEvent);
	if (it != mEvents.end())
	{
		auto& receivers = it->second;
		for (uint32_t i = 0; i < mEvents.size(); i++)
		{
			receivers[i]();
		}
	}
}

void EventDispatcher::RegisterReceiver(DispatchEvents aEvent, std::function<void()> aCallback)
{
	mEvents[aEvent].push_back(aCallback);
}
