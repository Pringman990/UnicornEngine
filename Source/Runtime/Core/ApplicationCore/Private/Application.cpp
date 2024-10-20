#include "Application.h"

GenericApplication* Application::sGenericApplication = nullptr;

Application::Application()
{
}

Application::~Application()
{
	
}

GenericApplication* Application::_CreateApp()
{
	sGenericApplication = GenericApplication::Create();
	return sGenericApplication;
}

void Application::Shutdown()
{
	delete sGenericApplication;
	sGenericApplication = nullptr;
}
