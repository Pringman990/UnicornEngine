#include "pch.h"
#include "Windows/WindowsApplication.h"

#include <dwmapi.h>
#include <Windows.h>
#include <EventDispatcher.h>


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
	mWinProcObservers.Clear();
	winInternal::windowsApplication = nullptr;
}

bool WindowsApplication::Init()
{
	if (!CreateWindow())
	{
		LOG_CORE_INFO("Failed To Create Windows Window");
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
		EventDispatcher::GetDispatcher().Dispatch(DispatchEvents::eEngineExit);
}

void WindowsApplication::AddWinProcObserver(IWindowsMessageObserver* aObserver)
{
	mWinProcObservers.Add(aObserver);
}

void WindowsApplication::RemoveWinProcObserver(IWindowsMessageObserver* aObserver)
{
	mWinProcObservers.Erase(aObserver);
}

LRESULT WindowsApplication::ProccessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (uint32_t i = 0; i < mWinProcObservers.Size(); i++)
	{
		mWinProcObservers[i]->ProccessMessages(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_SIZE:
	{
		//DX11Renderer::GetInstance().ResizeBackBuffer(LOWORD(lParam), HIWORD(lParam));
		break;
	}
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
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = mWindowInfo.name;
	if (!RegisterClassExW(&wc))
	{
		return false;
	}

	DWORD style = WS_OVERLAPPEDWINDOW;

	HWND hwnd = CreateWindowExW(0L,
		mWindowInfo.name,
		mWindowInfo.name,
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

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	return true;
}

IWindowInfo& WindowsApplication::GetWindowInfo()
{
	return mWindowInfo;
}

WindowWindowInfo& WindowsApplication::GetWindowsWindowInfo()
{
	return mWindowInfo;
}
