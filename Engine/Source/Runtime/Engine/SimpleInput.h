#pragma once
#include "SDL3/SDL_events.h"

namespace simpleInput
{
    void BeginFrame();
    void ProcessEvents(const SDL_Event& event);

    inline f32 MouseWheelDelta{};
    inline bool LeftMouseButtonPressed = false;
}
