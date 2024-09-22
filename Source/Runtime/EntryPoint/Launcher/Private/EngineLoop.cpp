#include "EngineLoop.h"

#include <Application.h>
#include <Generic/GenericApplication.h>
#include <EventDispatcher.h>

#include <Logger.h>
#include <Timer.h>

EngineLoop::EngineLoop()
	:
	mShouldExit(false),
	mGenericApplication(nullptr)
{
}

EngineLoop::~EngineLoop()
{
	EventDispatcher::ShutDown();
	Application::Shutdown();
}

bool EngineLoop::Init()
{
	TIMER_START_READING("__Engine Loop Init__");
	LOG_CORE_INFO("Engine Loop Starting Init");

	EventDispatcher::GetDispatcher().RegisterReceiver(DispatchEvents::eEngineExit, [&]() { RequestExit(); });

	mGenericApplication = Application::_CreateApp();

	if (!mGenericApplication->Init())
	{
		LOG_CORE_INFO("Engine Loop Failed To Create Application");
		assert(false);
		return false;
	}

	float initTime = TIMER_END_READING("__Engine Loop Init__");
	LOG_CORE_INFO("Engine Loop has finished Initilize, it took: {:0.7f}s", initTime);
	return true;
}

void EngineLoop::Update()
{
	mGenericApplication->Update();
}

void EngineLoop::RequestExit()
{
	LOG_CORE_INFO("Engine Loop exit has been requested");
	mShouldExit = true;
}

bool EngineLoop::EngineLoopClose() const
{
	return !mShouldExit;
}
