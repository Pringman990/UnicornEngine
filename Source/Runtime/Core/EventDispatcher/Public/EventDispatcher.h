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
		if (!_sInstance)
			_sInstance = new EventDispatcher();
		return *_sInstance;
	}

	static void ShutDown();

	void Dispatch(DispatchEvents aEvent);
	void RegisterReceiver(DispatchEvents aEvent, TFunction<void> aCallback);

private:
	EventDispatcher();
	~EventDispatcher();
private:
	static EventDispatcher* _sInstance;

	TUMap<DispatchEvents, TVector<TFunction<void>>> mEvents;

};