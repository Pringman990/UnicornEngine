#pragma once
#include "Application/Generic/IWindow.h"
#include <Windows.h>

struct WindowsWindowInfo : public IWindowInfo
{
	HWND windowHandle = nullptr;
	HINSTANCE hInstance = nullptr;
};