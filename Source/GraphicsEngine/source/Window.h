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
		Vector2 size = { };
		Vector2 resolution = { };
		HWND currentWindow = { };
		DWORD style = { };
		WindowType type = { };
		const wchar_t* name = L"";
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