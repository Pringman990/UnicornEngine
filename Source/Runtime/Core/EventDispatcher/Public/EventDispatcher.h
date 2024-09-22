#pragma once
#include <TUMap.h>
#include <TFunction.h>
#include <TVector.h>

enum class DispatchEvents
{
	eEngineExit
};

class EventDispatcher
{
public:
	static EventDispatcher& GetDispatcher()
	{
		if (!mInstance)
			mInstance = new EventDispatcher();
		return *mInstance;
	}

	static void ShutDown();

	void Dispatch(DispatchEvents aEvent);
	void RegisterReceiver(DispatchEvents aEvent, TFunction<void> aCallback);

private:
	EventDispatcher();
	~EventDispatcher();
private:
	static EventDispatcher* mInstance;

	TUMap<DispatchEvents, TVector<TFunction<void>>> mEvents;

};