#include "Application/Application.h"

REGISTER_ENGINE_SUBSYSTEM(Application)

Application::Application()
{
}

Application::~Application()
{
	delete mGenericApplication;
	mGenericApplication = nullptr;
}

GenericApplication* Application::_CreateApplication()
{
	mGenericApplication = GenericApplication::Create();
	return mGenericApplication;
}
