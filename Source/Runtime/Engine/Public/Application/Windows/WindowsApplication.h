#pragma once
#include "Application/Generic/GenericApplication.h"
#include <Application/Windows/WindowsWindowInfo.h>

class WindowsApplication : public GenericApplication
{
public:
	WindowsApplication();
	~WindowsApplication() override;

	ENGINE_API bool Init() override;
	ENGINE_API void Update() override;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT ProccessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#undef CreateWindow
	ENGINE_API bool CreateWindow();

	ENGINE_API IWindowInfo& GetWindowInfo() override;
	ENGINE_API WindowsWindowInfo& GetWindowsWindowInfo();

	MultiNotifierBoolRetArgs<HWND, UINT, WPARAM, LPARAM> OnWndProc;

private:
	WindowsWindowInfo mWindowInfo;
};