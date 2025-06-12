#include "pch.h"
#include "Application/Windows/WindowsApplication.h"

#include <dwmapi.h>
#include <Windows.h>

#ifdef _Win64
namespace winInternal
{
	/// <summary>
	/// Because WinProc is not a part of the class
	/// we have to expose a varible for it to use
	/// </summary>
	WindowsApplication* windowsApplication = nullptr;
}

WindowsApplication::WindowsApplication()
{
	winInternal::windowsApplication = this;
}

WindowsApplication::~WindowsApplication()
{
	winInternal::windowsApplication = nullptr;
}

bool WindowsApplication::Init()
{
	if (!CreateWindow())
	{
		_LOG_CORE_INFO("Failed To Create Windows Window");
		assert(false);
		return 0;
	}

	return true;
}

void WindowsApplication::Update()
{
	bool close = false;
	MSG msg = {};
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			close = true;
		}
	}

	if (close)
		OnApplicationRequestExist.Notify();
}

LRESULT WindowsApplication::ProccessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_DWMNCRENDERINGCHANGED:
	{
		//COLORREF DARK_COLOR = 0x00000000;
		//DwmSetWindowAttribute(hWnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR, &DARK_COLOR, sizeof(DARK_COLOR));

		break;
	}
	case WM_ACTIVATE:
	{

		break;
	}
	case WM_SIZE:
	{
		OnWindowResizeEvent.Notify(LOWORD(lParam), HIWORD(wParam));
		break;
	}
	//case WM_NCHITTEST:
	//{
	//	RECT rect;
	//	GetClientRect(hWnd, &rect);
	//	POINT cursor;
	//	GetCursorPos(&cursor);
	//	ScreenToClient(hWnd, &cursor);

	//	if (cursor.y >= rect.top && cursor.y < rect.top + 30) // Custom title bar height
	//	{
	//		return HTCAPTION;
	//	}
	//	break;
	//}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT WindowsApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//This should never be nullptr as the function is not registered if there is no instance
	if (winInternal::windowsApplication == nullptr)
		return 0;

	LRESULT result = winInternal::windowsApplication->ProccessWindowsMessages(hWnd, message, wParam, lParam);
	return result;
}

bool WindowsApplication::CreateWindow()
{
	std::wstring w = StringToWString(mWindowInfo.name);
	const wchar_t* name = w.c_str();

	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = name;
	if (!RegisterClassExW(&wc))
	{
		return false;
	}

	DWORD style = WS_OVERLAPPEDWINDOW;

	HWND hwnd = CreateWindowExW(0L,
		name,
		name,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(int)mWindowInfo.windowWidth,
		(int)mWindowInfo.windowHeight,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if (!hwnd)
		return false;

	mWindowInfo.windowHandle = hwnd;
	mWindowInfo.hInstance = wc.hInstance;

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	return true;
}

IWindowInfo& WindowsApplication::GetWindowInfo()
{
	return mWindowInfo;
}

WindowsWindowInfo& WindowsApplication::GetWindowsWindowInfo()
{
	return mWindowInfo;
}

#else
WindowsApplication::WindowsApplication()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
}

WindowsApplication::~WindowsApplication()
{
}

bool WindowsApplication::Init()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
	return false;
}

void WindowsApplication::Update()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
}

LRESULT WindowsApplication::ProccessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
	return 0;
}

LRESULT WindowsApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
	return 0;
}

bool WindowsApplication::CreateWindow()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);

	return false;
}

IWindowInfo& WindowsApplication::GetWindowInfo()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
	return mWindowInfo;
}

WindowsWindowInfo& WindowsApplication::GetWindowsWindowInfo()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
	return mWindowInfo;
}
#endif