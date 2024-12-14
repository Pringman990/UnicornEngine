#pragma once
#include <Singleton.h>

enum class DispatchEvents
{
	eEngineExit
};

class EventDispatcher : public Singleton<EventDispatcher>
{
public:
	void Dispatch(DispatchEvents aEvent);
	void RegisterReceiver(DispatchEvents aEvent, std::function<void()> aCallback);

private:
	friend class Singleton<EventDispatcher>;
	EventDispatcher();
	~EventDispatcher() override;
private:

	std::unordered_map<DispatchEvents, std::vector<std::function<void()>>> mEvents;

};