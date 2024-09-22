#pragma once
#include "Generic/GenericApplication.h"
#include <Windows/IWindowsMessageObserver.h>
#include <Windows/WindowsWindowInfo.h>

class WindowsApplication : public GenericApplication
{
public:
	WindowsApplication();
	~WindowsApplication() override;

	bool Init() override;
	void Update() override;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT ProccessWindowsMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	void AddWinProcObserver(IWindowsMessageObserver* aObserver);
	void RemoveWinProcObserver(IWindowsMessageObserver* aObserver);

#undef CreateWindow
	bool CreateWindow();

	IWindowInfo& GetWindowInfo() override;
	WindowWindowInfo& GetWindowsWindowInfo();

private:
	TVector<IWindowsMessageObserver*> mWinProcObservers;
	WindowWindowInfo mWindowInfo;
};