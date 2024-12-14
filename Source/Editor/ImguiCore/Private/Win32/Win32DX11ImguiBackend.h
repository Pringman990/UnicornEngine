#pragma once
#include "IImguiBackend.h"
#include "Windows/IWindowsMessageObserver.h"

class Win32DX11ImguiBackend : public IImguiBackend, public IWindowsMessageObserver
{
public:
	Win32DX11ImguiBackend();
	~Win32DX11ImguiBackend() override;

	virtual bool Init() override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;

	virtual void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:

};