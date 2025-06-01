#pragma once
#include "Application/Generic/GenericApplication.h"
#include <Application/Windows/WindowsWindowInfo.h>

class WindowsApplication : public GenericApplication
{
public:
	WindowsApplication();
	~WindowsApplication() override;

	bool Init() override;
	void Update() override;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT ProccessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#undef CreateWindow
	bool CreateWindow();

	IWindowInfo& GetWindowInfo() override;
	WindowsWindowInfo& GetWindowsWindowInfo();

private:
	WindowsWindowInfo mWindowInfo;
};