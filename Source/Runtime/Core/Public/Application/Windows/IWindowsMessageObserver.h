#pragma once
#include <Windows.h>

class IWindowsMessageObserver
{
public:
	virtual void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
};