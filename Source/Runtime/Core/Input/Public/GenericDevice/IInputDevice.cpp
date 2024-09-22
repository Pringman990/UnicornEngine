#include "pch.h"
#include "IInputDevice.h"

#include "InputMapper.h"

#ifdef _Win32
#include "WindowsDevice/WindowsInputDevice.h"
#endif // _Win32


IInputDevice* IInputDevice::Create(InputMapper* aMapper)
{
#ifdef _Win32
	return new WindowsInputDevice(aMapper);
#endif // _Win32
}

void IInputDevice::GetKeys(TUMap<FString, uint32_t>& /*aStringKeyMap*/)
{

}
