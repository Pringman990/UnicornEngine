#pragma once
#include "Generic/IWindow.h"
#include <Windows.h>

struct WindowWindowInfo : public IWindowInfo
{
	HWND windowHandle = nullptr;
};