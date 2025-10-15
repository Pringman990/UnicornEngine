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

	ENGINE_API bool Init() override;
	ENGINE_API void Update() override;

#undef CreateWindow
	ENGINE_API bool CreateWindow();

	ENGINE_API IWindowInfo& GetWindowInfo() override;

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

	ENGINE_API bool Init() override;
	ENGINE_API void Update() override;

	ENGINE_API IWindowInfo& GetWindowInfo() override;

private:
	SDLWindowInfo mWindowInfo;
};

#endif