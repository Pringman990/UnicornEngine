#include "GraphicsPch.h"
#include "Window.h"

#include <dwmapi.h>
#include <source/input/InputManager.h>
#include "DX11.h"

#ifdef _DEBUG
#include <imgui/imgui_impl_win32.h>
#endif


WinAPI::Window::Window()
{
}

WinAPI::Window::~Window()
{
}

bool WinAPI::Window::Init()
{
	mWindowInfo.size = {1200, 720};
	mWindowInfo.resolution = {};
	mWindowInfo.name = L"Unicorn";
	mWindowInfo.type = WindowType::eWindow;
	mWindowInfo.style = WS_OVERLAPPEDWINDOW /*& ~(WS_CAPTION | WS_SYSMENU)*/;

	if (!CreateWindow())
		return false;

	CreateConsoleWindow();

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool WinAPI::Window::CreateWindow()
{
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = mWindowInfo.name;
	RegisterClassExW(&wc);

	HWND hwnd = CreateWindowExW(0L,
		mWindowInfo.name,
		mWindowInfo.name,
		mWindowInfo.style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(int)mWindowInfo.size.x,
		(int)mWindowInfo.size.y,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);


	if (!hwnd)
		return false;

	mWindowInfo.currentWindow = hwnd;
	
	//SetWindowLong(hwnd, GWL_STYLE, 0);
	
	ShowWindow(hwnd, SW_SHOWDEFAULT);

	return true;
}

void WinAPI::Window::CreateConsoleWindow()
{
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONIN$", "r", stdin);
	SetConsoleTitle(L"Console Window");
}

const WinAPI::WindowInfo& WinAPI::Window::GetWindowInfo() const
{
	return mWindowInfo;
}

void WinAPI::Window::SetWindowInfo(const WindowInfo& aInfo)
{
	mWindowInfo = aInfo;
}
#ifdef _DEBUG
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
#endif

	Input::UpdateEvents(message, wParam, lParam);

	switch (message)
	{
	case WM_SIZE:
	{
		if (Engine::GetGraphicsEngine().GetDX11())
		{
			Engine::GetGraphicsEngine().GetDX11()->ResizeBackBuffer(LOWORD(lParam), HIWORD(lParam));
		}
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
