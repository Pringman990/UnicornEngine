#include "pch.h"
#include "SimpleInput.h"


void simpleInput::BeginFrame()
{
    MouseWheelDelta = 0.0f;
    LeftMouseButtonPressed = false;
    LeftMouseButtonReleased = false;

    RightMouseButtonPressed = false;
    RightMouseButtonReleased = false;
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
                LeftMouseButtonDown = true;
            }
            else if (event.button.button == SDL_BUTTON_RIGHT)
            {
                RightMouseButtonPressed = true;
                RightMouseButtonDown = true;
            }

            break;
        }
    case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                LeftMouseButtonReleased = true;
                LeftMouseButtonDown = false;
            }
            else if (event.button.button == SDL_BUTTON_RIGHT)
            {
                RightMouseButtonPressed = true;
                RightMouseButtonDown = false;
            }

            break;
        }
    default: ;
    }
}