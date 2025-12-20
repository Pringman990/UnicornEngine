#include "Application/Application.h"

Application* Application::sInstance = nullptr;
REGISTER_ENGINE_SUBSYSTEM(Application)

Application::Application()
{
#ifdef _DEBUG
	ASSERT(sInstance == nullptr, "The instance was not null and we are trying to set it again");
#endif
	sInstance = this;
}

Application::~Application()
{
	delete mGenericApplication;
	mGenericApplication = nullptr;

	if (sInstance == this)
		sInstance = nullptr;
}

ENGINE_API Application* Application::Instance()
{
#ifdef _DEBUG
	ASSERT(sInstance, "Instance was accessed before/after it was created/destroyed");
#endif
	return sInstance;
}

GenericApplication* Application::_CreateApplication()
{
	mGenericApplication = GenericApplication::Create();
	return mGenericApplication;
}
