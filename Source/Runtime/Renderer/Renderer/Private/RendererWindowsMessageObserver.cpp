#include "pch.h"
#include "RendererWindowsMessageObserver.h"

#include "RenderTarget.h"

RendererWindowsMessageObserver::RendererWindowsMessageObserver()
{
}

RendererWindowsMessageObserver::~RendererWindowsMessageObserver()
{
}

void RendererWindowsMessageObserver::ProccessMessages(HWND /*hWnd*/, UINT message, WPARAM /*wParam*/, LPARAM lParam)
{
	if (message == WM_SIZE)
	{
		Renderer::GetInstance()->ResizeBackbuffer(LOWORD(lParam), HIWORD(lParam));
	}
}
