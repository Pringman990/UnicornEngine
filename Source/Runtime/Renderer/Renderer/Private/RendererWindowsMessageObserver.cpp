#include "pch.h"
#include "RendererWindowsMessageObserver.h"

#include "RenderTarget.h"

RendererWindowsMessageObserver::RendererWindowsMessageObserver()
{
}

RendererWindowsMessageObserver::~RendererWindowsMessageObserver()
{
}

void RendererWindowsMessageObserver::ProccessMessages(HWND /*hWnd*/, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_ENTERSIZEMOVE:
		{
			Renderer::GetInstance()->SetIsResizingBackbuffer(true);
			break;
		}
		case WM_EXITSIZEMOVE:
		{
			Renderer::GetInstance()->SetIsResizingBackbuffer(false);
			Renderer::GetInstance()->ResizeBackbuffer(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_SIZE:
		{
			if (!Renderer::GetInstance()->IsResizingBackbuffer() && wParam != SIZE_MINIMIZED)
			{
				Renderer::GetInstance()->ResizeBackbuffer(LOWORD(lParam), HIWORD(lParam));
			}
			break;
		}
	}
}
