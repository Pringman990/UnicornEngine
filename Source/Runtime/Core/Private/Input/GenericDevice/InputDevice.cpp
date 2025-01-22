#include "pch.h"
#include "Input/GenericDevice/InputDevice.h"

#include "Input/InputMapper.h"

#ifdef _Win32
#include "Input/WindowsDevice/WindowsInputDevice.h"
#endif // _Win32


InputDevice* InputDevice::Create(InputMapper* aMapper)
{
	return new InputDeviceAPI(aMapper);
}