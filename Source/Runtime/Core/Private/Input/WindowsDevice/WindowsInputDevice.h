#pragma once
#include "Input/GenericDevice/InputDevice.h"

class WindowsInputDevice
	:
	public InputDevice
{
public:
	WindowsInputDevice(InputMapper* aMapper);
	~WindowsInputDevice();

	virtual void Init() override;
	virtual void Update() override;

	virtual void GetKeys(UnorderedMap<String, uint32>& aStringKeyMap) override;

	virtual void CaptureMouse() override;
	virtual void ReleaseMouse() override;

	virtual void HideMouse() override;
	virtual void ShowMouse() override;

	virtual Vector2 GetMouseDelta() override;

private:
	void ProccessVirtualKeyboardKeys(USHORT aVirtualKey, bool isDown);
	void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	Vector2 mMouseDelta;
	Vector2 mTentativeMouseDelta;
};

#ifdef _Win32
typedef WindowsInputDevice InputDeviceAPI;
#endif // _Win32