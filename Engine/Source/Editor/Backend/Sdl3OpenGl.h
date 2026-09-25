#pragma once
#include "SDL3/SDL_events.h"

class SDL3OpenGL
{
public:
    SDL3OpenGL();
    ~SDL3OpenGL();

    bool Init();

    void BeginFrame();
    void RenderFrame();
    void EndFrame();

private:
    void ProcessEvent(SDL_Event event);
private:
};
