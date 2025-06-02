#include "pch.h"
#include "Application/SDL/SDLApplication.h"

#ifdef _SDL

SDLApplication::SDLApplication()
	:
	mWindow(nullptr)
{
}

SDLApplication::~SDLApplication()
{
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

bool SDLApplication::Init()
{
	if (!CreateWindow())
	{
		_LOG_CORE_INFO("Failed To Create SDL Window");
		assert(false);
		return 0;
	}

	return true;
}

void SDLApplication::Update()
{
	bool close = false;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			close = true;
		}

		HandleEvent(event);
	}

	if (close)
		OnApplicationRequestExist.Notify();
}

bool SDLApplication::CreateWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		_LOG_CORE_CRITICAL("Failed to init SDL: {}", SDL_GetError());
		assert(false);
		return false;
	}

	mWindow = SDL_CreateWindow(
		mWindowInfo.name,
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		mWindowInfo.windowWidth,
		mWindowInfo.windowHeight,
		SDL_WINDOW_SHOWN
		);

	if (mWindow == nullptr)
	{
		_LOG_CORE_CRITICAL("Failed to create SDL window: {}", SDL_GetError());
		assert(false);
		return false;
	}

	return true;
}

void SDLApplication::HandleEvent(SDL_Event Event)
{
	switch (Event.type)
	{
	case SDL_WINDOWEVENT:
	{
		if (Event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			OnWindowResizeEvent.Notify(static_cast<int32>(Event.window.data1), static_cast<int32>(Event.window.data2));
		}
		break;
	}
	default:
		break;
	}
}

IWindowInfo& SDLApplication::GetWindowInfo()
{
	return mWindowInfo;
}

#else
SDLApplication::SDLApplication()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
}

SDLApplication::~SDLApplication()
{
}

bool SDLApplication::Init()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
	return false;
}

void SDLApplication::Update()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
}

IWindowInfo& SDLApplication::GetWindowInfo()
{
	_LOG_CORE_INFO("We are not using SDL");
	assert(false);
	return mWindowInfo;
}
#endif