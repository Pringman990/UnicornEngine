#pragma once
#include "IImguiBackend.h"

class Win32DX12ImguiBackend : public IImguiBackend
{
public:
	Win32DX12ImguiBackend();
	~Win32DX12ImguiBackend() override;

	virtual bool Init() override;
	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;

	void ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void ResizeBackBuffer(Vector2 aNewSize);
};