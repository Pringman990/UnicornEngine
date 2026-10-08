//
// Created on 2026-07-02.
//
#include "../pch.h"
#include "Application.h"

#include "../Core/Logging/Logs.h"

namespace
{
    Key TranslateKey(SDL_Scancode key)
    {
        if (key >= SDL_SCANCODE_A && key <= SDL_SCANCODE_Z)
            return static_cast<Key>(static_cast<int>(Key::A) + key - SDL_SCANCODE_A);
        switch (key)
        {
        case SDL_SCANCODE_SPACE: return Key::Space;
        case SDL_SCANCODE_ESCAPE: return Key::Escape;
        case SDL_SCANCODE_RETURN: return Key::Enter;
        case SDL_SCANCODE_TAB: return Key::Tab;
        case SDL_SCANCODE_BACKSPACE: return Key::Backspace;
        case SDL_SCANCODE_LEFT: return Key::Left;
        case SDL_SCANCODE_RIGHT: return Key::Right;
        case SDL_SCANCODE_UP: return Key::Up;
        case SDL_SCANCODE_DOWN: return Key::Down;
        case SDL_SCANCODE_LSHIFT: return Key::LeftShift;
        case SDL_SCANCODE_RSHIFT: return Key::RightShift;
        case SDL_SCANCODE_LCTRL: return Key::LeftControl;
        case SDL_SCANCODE_RCTRL: return Key::RightControl;
        case SDL_SCANCODE_LALT: return Key::LeftAlt;
        case SDL_SCANCODE_RALT: return Key::RightAlt;
        default: return Key::Unknown;
        }
    }

    MouseButton TranslateButton(uint8 button)
    {
        switch (button)
        {
        case SDL_BUTTON_LEFT: return MouseButton::Left;
        case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
        case SDL_BUTTON_RIGHT: return MouseButton::Right;
        default: return MouseButton::None;
        }
    }
}

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
        FATAL("SDL_Init failed: {}", SDL_GetError());
    }

    LOG_INFO("Video driver: {}", SDL_GetCurrentVideoDriver());

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
    mInput.BeginFrame();
    glm::vec2 motionDelta(0.0f);

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        // The editor's platform backend still needs native events, including its extra windows.
        mProcessEventsNotifier.Notify(event);
        if (event.type != SDL_EVENT_QUIT && event.common.type >= SDL_EVENT_WINDOW_FIRST &&
            event.common.type <= SDL_EVENT_WINDOW_LAST && event.window.windowID != SDL_GetWindowID(mWindow))
            continue;
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
        case SDL_EVENT_WINDOW_RESIZED:
            mInfo.viewportWidth = event.window.data1;
            mInfo.viewportHeight = event.window.data2;
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            mFocused = false;
            mInput.Push({.type = InputType::FocusLost});
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            mFocused = true;
            break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (mFocused && event.key.windowID == SDL_GetWindowID(mWindow))
                mInput.Push({.type = event.type == SDL_EVENT_KEY_DOWN ? InputType::KeyDown : InputType::KeyUp,
                             .key = TranslateKey(event.key.scancode), .repeat = event.key.repeat});
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (mFocused && event.button.windowID == SDL_GetWindowID(mWindow))
                mInput.Push({.type = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? InputType::MouseDown : InputType::MouseUp,
                             .button = TranslateButton(event.button.button), .position = {event.button.x, event.button.y}});
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (mFocused && event.motion.windowID == SDL_GetWindowID(mWindow))
                motionDelta += glm::vec2(event.motion.xrel, event.motion.yrel);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if (mFocused && event.wheel.windowID == SDL_GetWindowID(mWindow))
            {
                const f32 direction = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
                mInput.Push({.type = InputType::Scroll, .position = {event.wheel.mouse_x, event.wheel.mouse_y},
                             .delta = direction * glm::vec2(event.wheel.x, event.wheel.y)});
            }
            break;
        default: ;
        }
    }

    // Coalesce motion to one update per frame. Buttons retain their event positions.
    // Refresh even a stationary pointer so newly opened UI blocks edge scrolling.
    if (mFocused && SDL_GetMouseFocus() == mWindow)
    {
        glm::vec2 mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);
        mInput.Push({.type = InputType::MouseMove, .position = mouse, .delta = motionDelta});
    }
}
