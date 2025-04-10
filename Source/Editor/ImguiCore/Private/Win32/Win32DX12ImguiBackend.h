#pragma once
#include "IImguiBackend.h"
#include "Application/Windows/IWindowsMessageObserver.h"

class Win32DX12ImguiBackend : public IImguiBackend, public IWindowsMessageObserver
{
public:
	Win32DX12ImguiBackend();
	~Win32DX12ImguiBackend() override;

	virtual bool Init() override;
	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;

	virtual void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	void ResizeBackBuffer();
};