#pragma once
#include <Application/Windows/IWindowsMessageObserver.h>

class RendererWindowsMessageObserver : public IWindowsMessageObserver
{
public:
	RendererWindowsMessageObserver();
	~RendererWindowsMessageObserver();

	virtual void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:

};