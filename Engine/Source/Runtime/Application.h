#pragma once
#include "Core/ServiceRegistry.h"
#include "SDL3/SDL.h"

struct ApplicationWindowCreateInfo
{
    SDL_WindowFlags flags;

    uint32_t baseWidth = 1280;
    uint32_t baseHeight = 720;

    bool useOpenGL = false;

    // OpenGL-specific
    int major = 0;
    int minor = 0;
    int profile = 0;

    bool doubleBuffer = false;
    int depthBits = 0;
    int stencilBits = 0;
};

struct ApplicationInfo
{
    uint32 windowWidth;
    uint32 windowHeight;

    int32 viewportWidth;
    int32 viewportHeight;
};

class Application
{
    INIT_SERVICE(Application)

public:

    Application();
    ~Application();

    void Destroy();
    bool Init(SDL_InitFlags flags, const ApplicationWindowCreateInfo& windowCreateInfo);

    void Tick();

    SDL_Window* GetWindow() const { return mWindow; };

    const ApplicationInfo& GetInfo() const { return mInfo; };

private:

    /**
     * @warning
     * Window can be null if SDL_InitFlags SDL_INIT_VIDEO has not been set.
     */
    SDL_Window* mWindow = nullptr;

    bool mShouldClose = false;

    ApplicationInfo mInfo;
};
