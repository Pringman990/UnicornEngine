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
	ASSERT(false, "Called class has not implemented GenericApplication GetWindowInfo");
	
	IWindowInfo info;
	return info;
}
#pragma warning(pop)