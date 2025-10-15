#pragma once
#include <EngineMinimal.h>
#include "IImguiBackend.h"

class Win32DX11ImguiBackend : public IImguiBackend
{
public:
	Win32DX11ImguiBackend();
	~Win32DX11ImguiBackend() override;

	virtual bool Init() override;
	virtual void BeginFrame() override;
	virtual void RenderFrame() override;
	virtual void EndFrame() override;

	bool ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void ResizeBackBuffer(Vector2 NewSize);
};