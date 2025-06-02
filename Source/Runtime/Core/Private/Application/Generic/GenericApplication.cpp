#include "pch.h"
#include "Application/Generic/GenericApplication.h"
#include "Application/Windows/WindowsApplicationMisc.h"
#include "Application/SDL/SDLApplicationMisc.h"

GenericApplication::GenericApplication()
{
}

GenericApplication::~GenericApplication()
{
	OnApplicationRequestExist.RemoveAll();
	OnWindowResizeEvent.RemoveAll();
}

GenericApplication* GenericApplication::Create()
{
	return ApplicationMisc::Create();
}

bool GenericApplication::Init()
{
	return false;
}

void GenericApplication::Update()
{

}

#pragma warning(push)
#pragma warning(disable : 4172)
IWindowInfo& GenericApplication::GetWindowInfo()
{
	_LOG_CORE_WARNING("Called class has not implemented GenericApplication GetWindowInfo, Was this intended?");
	
	assert(false);
	
	IWindowInfo info;
	return info;
}
#pragma warning(pop)