#include "pch.h"
#include "GenericDevice/InputDevice.h"

#include "InputMapper.h"

#ifdef _Win32
#include "WindowsDevice/WindowsInputDevice.h"
#endif // _Win32


InputDevice* InputDevice::Create(InputMapper* aMapper)
{
	return new InputDeviceAPI(aMapper);
}