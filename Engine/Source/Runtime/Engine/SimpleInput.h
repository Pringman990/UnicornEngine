#pragma once
#include "SDL3/SDL_events.h"

namespace simpleInput
{
    void BeginFrame();
    void ProcessEvents(const SDL_Event& event);

    inline f32 MouseWheelDelta{};
    inline bool LeftMouseButtonPressed = false;
    inline bool LeftMouseButtonReleased = false;
    inline bool LeftMouseButtonDown = false;

    inline bool RightMouseButtonPressed = false;
    inline bool RightMouseButtonReleased = false;
    inline bool RightMouseButtonDown = false;
}
