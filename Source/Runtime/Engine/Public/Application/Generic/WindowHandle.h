#pragma once

#ifdef _WIN32
#include <Windows.h>
#elif defined(_SDL)
#include <SDL2/SDL.h>
#endif

struct WindowHandle
{
#ifdef _WIN32
	HWND hwnd = nullptr;
	operator HWND() const { return hwnd; }

#elif defined(_SLD)
	SDL_Window* hwnd = nullptr;
	operator SDL_Window*() const { return hwnd; }
#endif

};