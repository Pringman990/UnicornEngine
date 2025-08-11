#include "pch.h"
#include "WindowsInputDevice.h"

#include "Input/InputMapper.h"
#include <Application/Application.h>
#include <Application/Windows/WindowsApplication.h>

WindowsInputDevice::WindowsInputDevice(InputMapper* aMapper)
	:
	InputDevice(aMapper),
	mTentativeMouseDelta(Vector2(0, 0)),
	mMouseDelta(Vector2(0, 0))
{
}

WindowsInputDevice::~WindowsInputDevice()
{
}

void WindowsInputDevice::Init()
{
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD        ((USHORT) 0x06)
#endif

	WindowsApplication* application = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
	HWND hWnd = application->GetWindowsWindowInfo().windowHandle;

	//Keyboard setup
	RAWINPUTDEVICE rid[2];
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = hWnd;

	//Mouse setup
	rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[1].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = hWnd;

	BOOL result = RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
	if (result == 0)
	{
		_LOG_CORE_ERROR("Failed To Create Windows Input Device");
		return;
	}

	application->OnWndProc.AddRaw(this, &WindowsInputDevice::ProccessMessages);
}

void WindowsInputDevice::Update()
{
	mMouseDelta = mTentativeMouseDelta;
	mTentativeMouseDelta = { 0, 0 };
}

bool WindowsInputDevice::ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wParam;
	hWnd;

	if (message == WM_INPUT)
	{
		UINT dwSize = 0;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == nullptr)
			return false;

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		{
			_LOG_CORE_ERROR("GetRawInputData Did Not Return Correct Size");
		}

		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			RAWKEYBOARD rawKeyboard = raw->data.keyboard;
			USHORT virtualKey = rawKeyboard.VKey;
			USHORT flags = rawKeyboard.Flags;

			bool isKeyDown = !(flags & RI_KEY_BREAK);

			ProccessVirtualKeyboardKeys(virtualKey, isKeyDown);
		}

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			mTentativeMouseDelta.x += raw->data.mouse.lLastX;
			mTentativeMouseDelta.y += raw->data.mouse.lLastY;
		}

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			RAWMOUSE rawMouse = raw->data.mouse;

			if (rawMouse.usFlags == MOUSE_MOVE_RELATIVE) {
				//std::cout << "Mouse Move: X = " << rawMouse.lLastX << " Y = " << rawMouse.lLastY << std::endl;
			}

			if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
				//std::cout << "Left Mouse Button Down" << std::endl;
			}
			if (rawMouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
				//std::cout << "Left Mouse Button Up" << std::endl;
			}
			if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
				//std::cout << "Right Mouse Button Down" << std::endl;
			}
			if (rawMouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
				//std::cout << "Right Mouse Button Up" << std::endl;
			}
			if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL) {
				short wheelDelta = (short)rawMouse.usButtonData;
				wheelDelta;
				//std::cout << "Mouse Wheel: " << wheelDelta << std::endl;
			}
		}

		delete[] lpb;
	}

	return false;
}

void WindowsInputDevice::GetKeys(UnorderedMap<String, uint32>& StringKeyMap)
{
#define ADD_KEY_MAPPING(KEY, NAME) StringKeyMap.insert({#NAME, KEY}); 

	ADD_KEY_MAPPING(VK_LBUTTON, "LeftMouseButton");
	ADD_KEY_MAPPING(VK_RBUTTON, "RightMouseButton");
	ADD_KEY_MAPPING(VK_MBUTTON, "MiddleMouseButton");

	ADD_KEY_MAPPING(VK_XBUTTON1, "ThumbMouseButton");
	ADD_KEY_MAPPING(VK_XBUTTON2, "ThumbMouseButton2");

	ADD_KEY_MAPPING(VK_BACK, "BackSpace");
	ADD_KEY_MAPPING(VK_TAB, "Tab");
	ADD_KEY_MAPPING(VK_RETURN, "Enter");
	ADD_KEY_MAPPING(VK_PAUSE, "Pause");

	ADD_KEY_MAPPING(VK_CAPITAL, "CapsLock");
	ADD_KEY_MAPPING(VK_ESCAPE, "Escape");
	ADD_KEY_MAPPING(VK_SPACE, "SpaceBar");
	ADD_KEY_MAPPING(VK_PRIOR, "PageUp");
	ADD_KEY_MAPPING(VK_NEXT, "PageDown");
	ADD_KEY_MAPPING(VK_END, "End");
	ADD_KEY_MAPPING(VK_HOME, "Home");

	ADD_KEY_MAPPING(VK_LEFT, "Left");
	ADD_KEY_MAPPING(VK_UP, "Up");
	ADD_KEY_MAPPING(VK_RIGHT, "Right");
	ADD_KEY_MAPPING(VK_DOWN, "Down");

	ADD_KEY_MAPPING(VK_INSERT, "Insert");
	ADD_KEY_MAPPING(VK_DELETE, "Delete");

	ADD_KEY_MAPPING(VK_NUMPAD0, "NumPadZero");
	ADD_KEY_MAPPING(VK_NUMPAD1, "NumPadOne");
	ADD_KEY_MAPPING(VK_NUMPAD2, "NumPadTwo");
	ADD_KEY_MAPPING(VK_NUMPAD3, "NumPadThree");
	ADD_KEY_MAPPING(VK_NUMPAD4, "NumPadFour");
	ADD_KEY_MAPPING(VK_NUMPAD5, "NumPadFive");
	ADD_KEY_MAPPING(VK_NUMPAD6, "NumPadSix");
	ADD_KEY_MAPPING(VK_NUMPAD7, "NumPadSeven");
	ADD_KEY_MAPPING(VK_NUMPAD8, "NumPadEight");
	ADD_KEY_MAPPING(VK_NUMPAD9, "NumPadNine");

	ADD_KEY_MAPPING(VK_MULTIPLY, "Multiply");
	ADD_KEY_MAPPING(VK_ADD, "Add");
	ADD_KEY_MAPPING(VK_SUBTRACT, "Subtract");
	ADD_KEY_MAPPING(VK_DECIMAL, "Decimal");
	ADD_KEY_MAPPING(VK_DIVIDE, "Divide");

	ADD_KEY_MAPPING(VK_F1, "F1");
	ADD_KEY_MAPPING(VK_F2, "F2");
	ADD_KEY_MAPPING(VK_F3, "F3");
	ADD_KEY_MAPPING(VK_F4, "F4");
	ADD_KEY_MAPPING(VK_F5, "F5");
	ADD_KEY_MAPPING(VK_F6, "F6");
	ADD_KEY_MAPPING(VK_F7, "F7");
	ADD_KEY_MAPPING(VK_F8, "F8");
	ADD_KEY_MAPPING(VK_F9, "F9");
	ADD_KEY_MAPPING(VK_F10, "F10");
	ADD_KEY_MAPPING(VK_F11, "F11");
	ADD_KEY_MAPPING(VK_F12, "F12");

	ADD_KEY_MAPPING(VK_NUMLOCK, "NumLock");

	ADD_KEY_MAPPING(VK_SCROLL, "ScrollLock");

	ADD_KEY_MAPPING(VK_LSHIFT, "LeftShift");
	ADD_KEY_MAPPING(VK_RSHIFT, "RightShift");
	ADD_KEY_MAPPING(VK_LCONTROL, "LeftControl");
	ADD_KEY_MAPPING(VK_RCONTROL, "RightControl");
	ADD_KEY_MAPPING(VK_LMENU, "LeftAlt");
	ADD_KEY_MAPPING(VK_RMENU, "RightAlt");
	ADD_KEY_MAPPING(VK_LWIN, "LeftCommand");
	ADD_KEY_MAPPING(VK_RWIN, "RightCommand");

	ADD_KEY_MAPPING('0', "Zero");
	ADD_KEY_MAPPING('1', "One");
	ADD_KEY_MAPPING('2', "Two");
	ADD_KEY_MAPPING('3', "Three");
	ADD_KEY_MAPPING('4', "Four");
	ADD_KEY_MAPPING('5', "Five");
	ADD_KEY_MAPPING('6', "Six");
	ADD_KEY_MAPPING('7', "Seven");
	ADD_KEY_MAPPING('8', "Eight");
	ADD_KEY_MAPPING('9', "Nine");

	ADD_KEY_MAPPING('A', "A");
	ADD_KEY_MAPPING('B', "B");
	ADD_KEY_MAPPING('C', "C");
	ADD_KEY_MAPPING('D', "D");
	ADD_KEY_MAPPING('E', "E");
	ADD_KEY_MAPPING('F', "F");
	ADD_KEY_MAPPING('G', "G");
	ADD_KEY_MAPPING('H', "H");
	ADD_KEY_MAPPING('I', "I");
	ADD_KEY_MAPPING('J', "J");
	ADD_KEY_MAPPING('K', "K");
	ADD_KEY_MAPPING('L', "L");
	ADD_KEY_MAPPING('M', "M");
	ADD_KEY_MAPPING('N', "N");
	ADD_KEY_MAPPING('O', "O");
	ADD_KEY_MAPPING('P', "P");
	ADD_KEY_MAPPING('Q', "Q");
	ADD_KEY_MAPPING('R', "R");
	ADD_KEY_MAPPING('S', "S");
	ADD_KEY_MAPPING('T', "T");
	ADD_KEY_MAPPING('U', "U");
	ADD_KEY_MAPPING('V', "V");
	ADD_KEY_MAPPING('W', "W");
	ADD_KEY_MAPPING('X', "X");
	ADD_KEY_MAPPING('Y', "Y");
	ADD_KEY_MAPPING('Z', "Z");

	ADD_KEY_MAPPING('a', "A");
	ADD_KEY_MAPPING('b', "B");
	ADD_KEY_MAPPING('c', "C");
	ADD_KEY_MAPPING('d', "D");
	ADD_KEY_MAPPING('e', "E");
	ADD_KEY_MAPPING('f', "F");
	ADD_KEY_MAPPING('g', "G");
	ADD_KEY_MAPPING('h', "H");
	ADD_KEY_MAPPING('i', "I");
	ADD_KEY_MAPPING('j', "J");
	ADD_KEY_MAPPING('k', "K");
	ADD_KEY_MAPPING('l', "L");
	ADD_KEY_MAPPING('m', "M");
	ADD_KEY_MAPPING('n', "N");
	ADD_KEY_MAPPING('o', "O");
	ADD_KEY_MAPPING('p', "P");
	ADD_KEY_MAPPING('q', "Q");
	ADD_KEY_MAPPING('r', "R");
	ADD_KEY_MAPPING('s', "S");
	ADD_KEY_MAPPING('t', "T");
	ADD_KEY_MAPPING('u', "U");
	ADD_KEY_MAPPING('v', "V");
	ADD_KEY_MAPPING('w', "W");
	ADD_KEY_MAPPING('x', "X");
	ADD_KEY_MAPPING('y', "Y");
	ADD_KEY_MAPPING('z', "Z");

	ADD_KEY_MAPPING(';', "Semicolon");
	ADD_KEY_MAPPING('=', "Equals");
	ADD_KEY_MAPPING(',', "Comma");
	ADD_KEY_MAPPING('-', "Hyphen");
	ADD_KEY_MAPPING('.', "Period");
	ADD_KEY_MAPPING('/', "Slash");
	ADD_KEY_MAPPING('`', "Tilde");
	ADD_KEY_MAPPING('[', "LeftBracket");
	ADD_KEY_MAPPING('\\', "Backslash");
	ADD_KEY_MAPPING(']', "RightBracket");
	ADD_KEY_MAPPING('\'', "Apostrophe");
	ADD_KEY_MAPPING(' ', "SpaceBar");

	ADD_KEY_MAPPING('&', "Ampersand");
	ADD_KEY_MAPPING('*', "Asterix");
	ADD_KEY_MAPPING('^', "Caret");
	ADD_KEY_MAPPING(':', "Colon");
	ADD_KEY_MAPPING('$', "Dollar");
	ADD_KEY_MAPPING('!', "Exclamation");
	ADD_KEY_MAPPING('(', "LeftParantheses");
	ADD_KEY_MAPPING(')', "RightParantheses");
	ADD_KEY_MAPPING('"', "Quote");
	ADD_KEY_MAPPING('_', "Underscore");
	ADD_KEY_MAPPING(224, "A_AccentGrave");
	ADD_KEY_MAPPING(231, "C_Cedille");
	ADD_KEY_MAPPING(233, "E_AccentAigu");
	ADD_KEY_MAPPING(232, "E_AccentGrave");
	ADD_KEY_MAPPING(167, "Section");

}

void WindowsInputDevice::CaptureMouse()
{
	//WindowsApplication* application = static_cast<WindowsApplication*>(Application::Get()->GetApplication());

	//RECT clipRect;
	//HWND window = application->GetWindowsWindowInfo().windowHandle;
	//GetClientRect(window, &clipRect);

	//POINT upperLeft;
	//upperLeft.x = clipRect.left;
	//upperLeft.y = clipRect.top;

	//POINT lowerRight;
	//lowerRight.x = clipRect.right;
	//lowerRight.y = clipRect.bottom;

	//MapWindowPoints(window, nullptr, &upperLeft, 1);
	//MapWindowPoints(window, nullptr, &lowerRight, 1);

	//clipRect.left = upperLeft.x;
	//clipRect.top = upperLeft.y;
	//clipRect.right = lowerRight.x;
	//clipRect.bottom = lowerRight.y;

	//ClipCursor(&clipRect);
	WindowsApplication* application = static_cast<WindowsApplication*>(Application::Get()->GetApplication());

	// Get the window handle
	HWND window = application->GetWindowsWindowInfo().windowHandle;

	// Get the client area (the non-decorated area) of the window
	RECT clientRect;
	GetClientRect(window, &clientRect);

	// Map to screen coordinates
	POINT upperLeft = { clientRect.left, clientRect.top };
	POINT lowerRight = { clientRect.right, clientRect.bottom };
	MapWindowPoints(window, nullptr, &upperLeft, 1);
	MapWindowPoints(window, nullptr, &lowerRight, 1);

	// Get the screen coordinates of the window boundaries
	int windowLeft = upperLeft.x;
	int windowTop = upperLeft.y;
	int windowRight = lowerRight.x;
	int windowBottom = lowerRight.y;

	// Get the current mouse position in screen coordinates
	POINT mousePos;
	GetCursorPos(&mousePos);

	// If the mouse is outside the window boundaries, wrap it to the opposite side
	if (mousePos.x < windowLeft)
	{
		mousePos.x = windowRight - 1;  // Wrap to the right side
	}
	else if (mousePos.x >= windowRight)
	{
		mousePos.x = windowLeft;  // Wrap to the left side
	}

	if (mousePos.y < windowTop)
	{
		mousePos.y = windowBottom - 1;  // Wrap to the bottom
	}
	else if (mousePos.y >= windowBottom)
	{
		mousePos.y = windowTop;  // Wrap to the top
	}

	// Set the cursor to the new wrapped position
	SetCursorPos(mousePos.x, mousePos.y);
}

void WindowsInputDevice::ReleaseMouse()
{
	ClipCursor(nullptr);
}

void WindowsInputDevice::HideMouse()
{
	ShowCursor(false);
}

void WindowsInputDevice::ShowMouse()
{
	ShowCursor(true);
}

Vector2 WindowsInputDevice::GetMouseDelta()
{
	return mMouseDelta;
}

void WindowsInputDevice::ProccessVirtualKeyboardKeys(USHORT /*aVirtualKey*/, bool /*isDown*/)
{
	//eInputActionType actionType = eInputActionType::ePressed;
	//if (isDown == false)
	//	actionType = eInputActionType::eReleased;
	//

}
