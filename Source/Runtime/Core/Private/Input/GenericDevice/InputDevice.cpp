#include "pch.h"
#include "Input/GenericDevice/InputDevice.h"

#include "Input/InputMapper.h"

#ifdef _Win32
#include "Input/WindowsDevice/WindowsInputDevice.h"
#elif _SDL

#endif // _Win32


InputDevice* InputDevice::Create(InputMapper* aMapper)
{
	//TODO: add sdl
	//return new InputDeviceAPI(aMapper);
	return nullptr;
}