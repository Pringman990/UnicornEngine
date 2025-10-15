#pragma once
#include "Application/Generic/GenericApplication.h"

struct WindowsApplicationMisc
{
	ENGINE_API static GenericApplication* Create();
};

#ifdef _WIN32
typedef WindowsApplicationMisc ApplicationMisc;
#endif // _Win32