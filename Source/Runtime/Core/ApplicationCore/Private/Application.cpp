#include "Application.h"

GenericApplication* Application::mGenericApplication = nullptr;

Application::Application()
{
}

Application::~Application()
{
	
}

GenericApplication* Application::_CreateApp()
{
	mGenericApplication = GenericApplication::Create();
	return mGenericApplication;
}

void Application::Shutdown()
{
	delete mGenericApplication;
	mGenericApplication = nullptr;
}
