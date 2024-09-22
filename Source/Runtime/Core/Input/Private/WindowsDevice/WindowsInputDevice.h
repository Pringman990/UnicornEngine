#pragma once
#include "GenericDevice/IInputDevice.h"
#include <Windows/IWindowsMessageObserver.h>

class WindowsInputDevice
	:
	public IInputDevice,
	public IWindowsMessageObserver
{
public:
	WindowsInputDevice(InputMapper* aMapper);
	~WindowsInputDevice();

	virtual void Init() override;
	virtual void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual void GetKeys(TUMap<FString, uint32_t>& aStringKeyMap) override;

private:
	void ProccessVirtualKeyboardKeys(USHORT aVirtualKey, bool isDown);
};