#pragma once
#include "Application/Generic/GenericApplication.h"

struct SDLApplicationMisc
{
	ENGINE_API static GenericApplication* Create();
};

#ifdef _SDL
typedef SDLApplicationMisc ApplicationMisc;
#endif // _Win32