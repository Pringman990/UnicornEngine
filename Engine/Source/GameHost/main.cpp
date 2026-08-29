#include "pch.h"

#include <SDL3/SDL_main.h>

#include "Application.h"
#include "ConsoleLogSink.h"
#include "FrameData.h"
#include "Core/Logs.h"
#include "Renderer/Renderer.h"
#include "Sandbox.h"
#include "Core/ServiceRegistry.h"
#include "ECS/SystemManager.h"
#include "ECS/WorldManager.h"

#include "FileSystem.h"
#include "Core/ReflectionRegistry.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    GServiceRegistry = new ServiceRegistry();

    Refl::ReflectionRegistry reflRegistry;
    GServiceRegistry->RegisterService(&reflRegistry);

    Logger logger;
    GServiceRegistry->RegisterService(&logger);
    logger.AddSink<ConsoleLogSink>();

    FileSystem filesystem;
    GServiceRegistry->RegisterService(&filesystem);

    Application app;
    GServiceRegistry->RegisterService(&app);

    Renderer renderer;
    GServiceRegistry->RegisterService(&renderer);

    SystemManager systemManager;
    GServiceRegistry->RegisterService(&systemManager);

    WorldManager worldManager;
    GServiceRegistry->RegisterService(&worldManager);

    {
        const auto worldId = worldManager.CreateWorld();
        worldManager.SetActiveWorld(worldId);
    }

    ApplicationWindowCreateInfo windowCreateInfo{};
    windowCreateInfo.useOpenGL = true;
    windowCreateInfo.major = 4;
    windowCreateInfo.minor = 6;
    windowCreateInfo.profile = SDL_GL_CONTEXT_PROFILE_CORE;
    windowCreateInfo.doubleBuffer = true;
    windowCreateInfo.depthBits = 24;
    windowCreateInfo.stencilBits = 8;
    windowCreateInfo.flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (!app.Init(SDL_INIT_VIDEO, windowCreateInfo))
    {
        std::cerr << "App Init failed!" << std::endl;
        return 1;
    }

    renderer.Init(app);

    Sandbox sandbox;
    sandbox.Init();

    uint64_t previousTime = SDL_GetTicks();
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                {
                    running = false;
                    break;
                }
            default: ;
            }
        }

        const uint64_t currentTime = SDL_GetTicks();
        const float deltaTime = static_cast<float>(currentTime - previousTime) / 1000.f;
        previousTime = currentTime;

        FrameData frameData{};
        frameData.deltaTime = deltaTime;

        worldManager.GetActiveWorld().SwapEventBuffers();

        sandbox.Tick(systemManager, frameData, worldManager.GetActiveWorld());

        renderer.Swap(app);
    }

    worldManager.ClearWorlds();

    GServiceRegistry->InvalidateService<WorldManager>();
    GServiceRegistry->InvalidateService<Renderer>();
    GServiceRegistry->InvalidateService<Application>();

    return 0;
}
