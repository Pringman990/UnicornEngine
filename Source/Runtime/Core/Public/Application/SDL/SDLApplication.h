#pragma once
#include "Application/Generic/GenericApplication.h"
#include "Application/SDL/SDLWindowInfo.h"

#ifdef _SDL
#include <SDL2/SDL.h>

class SDLApplication : public GenericApplication
{
public:
	SDLApplication();
	~SDLApplication() override;

	bool Init() override;
	void Update() override;

#undef CreateWindow
	bool CreateWindow();

	IWindowInfo& GetWindowInfo() override;

private:
	void HandleEvent(SDL_Event Event);
private:
	SDLWindowInfo mWindowInfo;
	SDL_Window* mWindow;
};

#else

class SDLApplication : public GenericApplication
{
public:
	SDLApplication();
	~SDLApplication() override;

	bool Init() override;
	void Update() override;

	IWindowInfo& GetWindowInfo() override;

private:
	SDLWindowInfo mWindowInfo;
};

#endif