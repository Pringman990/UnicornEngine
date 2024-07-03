#include "EnginePch.h"
#include "InputManager.h"

#include <Windowsx.h>
#include <source/Window.h>


namespace Input
{
	std::bitset<256> mTentativeState = {};
	std::bitset<256> mCurrentState = {};
	std::bitset<256> mPreviousState = {};

	Vector2 mTentativeMousePosition = {};
	Vector2 mCurrentMousePosition = {};
	Vector2 mPreviousMousePosition = {};
	Vector2 mTentativeMouseDelta = {};
	Vector2 mMouseDelta = {};

	std::bitset<3> mCurrentMouseState = {};
	std::bitset<3> mPreviousMouseState = {};

	float mTentativeMouseWheelDelta = {};
	float mMouseWheelDelta = {};

	bool Input::IsKeyReleased(const int aKeyCode)
	{
		return !mCurrentState[aKeyCode] && mPreviousState[aKeyCode];
	}

	bool Input::IsKeyHeld(const int aKeyCode)
	{
		return mCurrentState[aKeyCode] && mPreviousState[aKeyCode];
	}

	bool Input::IsKeyPressed(const int aKeyCode)
	{
		return mCurrentState[aKeyCode] && !mPreviousState[aKeyCode];
	}

	Vector2 Input::GetMouseDelta()
	{
		return mMouseDelta;
	}

	Vector2 Input::GetMousePosition()
	{
		return Vector2(static_cast<float>(mCurrentMousePosition.x), static_cast<float>(Engine::GetGraphicsEngine().GetCurrentWindow().GetWindowInfo().resolution.y - mCurrentMousePosition.y));
	}

	float Input::GetScrollWheelDelta()
	{
		return mMouseWheelDelta;
	}

	void Input::SetMousePosition(Vector2 aPosition)
	{
		SetCursorPos(static_cast<int>(aPosition.x), static_cast<int>(aPosition.y));
	}

	void Input::ShowMouse()
	{
		ShowCursor(true);
	}

	void Input::HideMouse()
	{
		ShowCursor(false);
	}

	void Input::CaptureMouse()
	{
		RECT clipRect;
		HWND window = Engine::GetGraphicsEngine().GetCurrentWindow().GetWindowInfo().currentWindow;
		GetClientRect(window, &clipRect);

		POINT upperLeft;
		upperLeft.x = clipRect.left;
		upperLeft.y = clipRect.top;

		POINT lowerRight;
		lowerRight.x = clipRect.right;
		lowerRight.y = clipRect.bottom;

		MapWindowPoints(window, nullptr, &upperLeft, 1);
		MapWindowPoints(window, nullptr, &lowerRight, 1);

		clipRect.left = upperLeft.x;
		clipRect.top = upperLeft.y;
		clipRect.right = lowerRight.x;
		clipRect.bottom = lowerRight.y;

		ClipCursor(&clipRect);
	}

	void Input::ReleaseMouse()
	{
		ClipCursor(nullptr);
	}

	bool Input::Init()
	{
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		Rid[0].dwFlags = RIDEV_INPUTSINK;
		Rid[0].hwndTarget = Engine::GetGraphicsEngine().GetCurrentWindow().GetWindowInfo().currentWindow;
		BOOL result = RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
		if (result == 0)
		{
			std::cout << "failed to init input" << std::endl;
			return false;
		}
		
		return true;
	}

	bool Input::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_KEYDOWN:
			mTentativeState[wParam] = true;
			return true;
		case WM_KEYUP:
			mTentativeState[wParam] = false;
			return true;

		case WM_LBUTTONDOWN:
			mTentativeState[VK_LBUTTON] = true;
			return true;

		case WM_LBUTTONUP:
			mTentativeState[VK_LBUTTON] = false;
			return true;

		case WM_RBUTTONDOWN:
			mTentativeState[VK_RBUTTON] = true;
			return true;

		case WM_RBUTTONUP:
			mTentativeState[VK_RBUTTON] = false;
			return true;

		case WM_MBUTTONDOWN:
			mTentativeState[VK_MBUTTON] = true;
			return true;

		case WM_MBUTTONUP:
			mTentativeState[VK_MBUTTON] = false;
			return true;
		case WM_SYSKEYDOWN:
			mTentativeState[wParam] = true;
			return true;
		case WM_SYSKEYUP:
			mTentativeState[wParam] = false;
			return true;
		case WM_XBUTTONDOWN:
		{
			const int xButton = GET_XBUTTON_WPARAM(wParam);
			if (xButton == 1)
				mTentativeState[VK_XBUTTON1] = true;
			else
				mTentativeState[VK_XBUTTON2] = true;

			return true;
		}
		case WM_XBUTTONUP:
		{
			const int xButton = GET_XBUTTON_WPARAM(wParam);
			if (xButton == 1)
				mTentativeState[VK_XBUTTON1] = false;
			else
				mTentativeState[VK_XBUTTON2] = false;
			return true;
		}
		case WM_MOUSEWHEEL:
			mTentativeMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			return true;
		case WM_MOUSEMOVE:
		{
			const int xPos = GET_X_LPARAM(lParam);
			const int yPos = GET_Y_LPARAM(lParam);

			mTentativeMousePosition.x = static_cast<float>(xPos);
			mTentativeMousePosition.y = static_cast<float>(yPos);

			return true;
		}
		case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				mTentativeMouseDelta.x += raw->data.mouse.lLastX;
				mTentativeMouseDelta.y += raw->data.mouse.lLastY;
			}
			return true;
		}
		}

		return false;
	}

	void Input::Update()
	{
		mPreviousMousePosition = mCurrentMousePosition;
		mCurrentMousePosition = mTentativeMousePosition;

		mMouseDelta = mTentativeMouseDelta;
		mTentativeMouseDelta = { 0, 0 };

		mMouseWheelDelta = mTentativeMouseWheelDelta / abs(mTentativeMouseWheelDelta);
		mTentativeMouseWheelDelta = 0;

		mPreviousState = mCurrentState;
		mCurrentState = mTentativeState;
	}
}
