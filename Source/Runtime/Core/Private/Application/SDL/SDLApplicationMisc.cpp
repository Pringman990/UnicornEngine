#include "Application/SDL/SDLApplicationMisc.h"
#include "Application/SDL/SDLApplication.h"

GenericApplication* SDLApplicationMisc::Create()
{
	return new SDLApplication();
}
