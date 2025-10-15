#pragma once
#include "Application/Generic/IWindow.h"
#include "Application/Generic/WindowHandle.h"
#include <Windows.h>

struct WindowsWindowInfo : public IWindowInfo
{
	WindowHandle windowHandle;
	HINSTANCE hInstance = nullptr;
};