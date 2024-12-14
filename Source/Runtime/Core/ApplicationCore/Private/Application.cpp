#include "Application.h"

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
