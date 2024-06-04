#pragma once
#include <Windows.h>

#undef CreateWindow
namespace WinAPI
{
	enum class WindowType
	{
		eWindow,
		eBorderless,
		eFullscreen
	};

	struct WindowInfo
	{
		int width, height;
		int viewportWidth, viewportHeight;
		HWND currentWindow;
		DWORD style;
		WindowType type;
		wchar_t* name;
	};

	class Window
	{
	public:
		Window();
		~Window();

		bool Init();

		const WindowInfo& GetWindowInfo() const;
		void SetWindowInfo(const WindowInfo& aInfo);

	private:
		bool CreateWindow();
		void CreateConsoleWindow();
	private:
		WindowInfo mWindowInfo;
	};
}