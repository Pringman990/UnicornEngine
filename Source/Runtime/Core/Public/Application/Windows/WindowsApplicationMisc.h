#pragma once
#include "Application/Generic/GenericApplication.h"

struct WindowsApplicationMisc
{
	static GenericApplication* Create();
};

#ifdef _Win32
typedef WindowsApplicationMisc ApplicationMisc;
#endif // _Win32