#pragma once
#include <bitset>

namespace Input
{
	extern std::bitset<256> mTentativeState;
	extern std::bitset<256> mCurrentState;
	extern std::bitset<256> mPreviousState;

	extern Vector2 mTentativeMousePosition;
	extern Vector2 mCurrentMousePosition;
	extern Vector2 mPreviousMousePosition;
	extern Vector2 mTentativeMouseDelta;
	extern Vector2 mMouseDelta;

	extern float mTentativeMouseWheelDelta;
	extern float mMouseWheelDelta;

	extern bool IsKeyHeld(const int aKeyCode);
	extern bool IsKeyPressed(const int aKeyCode);
	extern bool IsKeyReleased(const int aKeyCode);

	extern Vector2 GetMouseDelta();
	extern Vector2 GetMousePosition();
	extern float GetScrollWheelDelta();

	extern void SetMousePosition(Vector2 aPosition);

	extern void ShowMouse();
	extern void HideMouse();

	extern void CaptureMouse();
	extern void ReleaseMouse();

	extern bool Init();

	extern bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	extern void Update();
}