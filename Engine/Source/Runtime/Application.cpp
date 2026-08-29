//
// Created on 2026-07-02.
//
#include "pch.h"
#include "Application.h"

#include "Core/Logs.h"

Application::Application()
    :
    mWindow(nullptr),
    mShouldClose(false)
{
}

Application::~Application()
{

}

void Application::Destroy()
{
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

bool Application::Init(SDL_InitFlags flags, const ApplicationWindowCreateInfo& windowCreateInfo)
{
    if(!SDL_Init(flags))
    {
        return false;
    }

    if((flags & SDL_INIT_VIDEO) != 0)
    {
        if (windowCreateInfo.useOpenGL)
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, windowCreateInfo.major);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, windowCreateInfo.minor);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, windowCreateInfo.profile);

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, windowCreateInfo.doubleBuffer ? 1 : 0);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, windowCreateInfo.depthBits);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, windowCreateInfo.stencilBits);
        }

        mWindow = SDL_CreateWindow("Unicorn Engine", windowCreateInfo.baseWidth, windowCreateInfo.baseHeight, windowCreateInfo.flags);
        if(mWindow == nullptr)
        {
            FATAL("Failed to create SDL window");
        }
        LOG_INFO("SDL window created");

        mInfo.windowWidth = windowCreateInfo.baseWidth;
        mInfo.windowHeight = windowCreateInfo.baseHeight;

        SDL_GetWindowSize(mWindow, &mInfo.viewportWidth, &mInfo.viewportHeight);
    }

    return true;
}

void Application::Tick()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_EVENT_QUIT:
            // OS / external quit request
            mShouldClose = true;
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            // User clicked X on window
            mShouldClose = true;
            break;
        default: ;
        }
    }
}
