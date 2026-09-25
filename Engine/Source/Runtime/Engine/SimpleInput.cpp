#include "pch.h"
#include "SimpleInput.h"


void simpleInput::BeginFrame()
{
    MouseWheelDelta = 0.0f;
    LeftMouseButtonPressed = false;
}

void simpleInput::ProcessEvents(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_EVENT_MOUSE_WHEEL:
        {
            MouseWheelDelta += event.wheel.y;
            break;
        }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                LeftMouseButtonPressed = true;
            }
        }
    default: ;
    }
}