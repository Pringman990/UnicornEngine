#include "pch.h"

#include <SDL3/SDL_main.h>

#include "Engine/Application.h"
#include "Engine/ConsoleLogSink.h"
#include "Engine/FrameData.h"
#include "Core/Logging/Logs.h"
#include "Renderer/Renderer.h"
#include "Sandbox.h"
#include "Core/ServiceRegistry.h"
#include "ECS/SystemManager.h"
#include "ECS/WorldManager.h"

#include "Engine/FileSystem.h"

#ifndef NDEBUG
#include <Editor.h>
#include <Backend/Sdl3OpenGl.h>
#include <EditorWindowManager.h>
#include <Windows/HierarchyEditorWindow.h>
#include <Windows/InspectorEditorWindow.h>
#include "EngineTypeDrawFuncs.h"
#endif

#include "../Runtime/Game/EngineComponents.h"
#include "Core/ReflectionRegistry.h"
#include "Game/ControllerManager.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    GServiceRegistry = new ServiceRegistry();

    refl::ReflectionRegistry reflRegistry;
    GServiceRegistry->RegisterService(&reflRegistry);

    refl::ReflectionRegistry::RegisterStandardTypes();
    RegisterEngineComponents();

    Logger logger;
    GServiceRegistry->RegisterService(&logger);
    logger.AddSink<ConsoleLogSink>();

    FileSystem filesystem;
    GServiceRegistry->RegisterService(&filesystem);

    filesystem.AddMount("engine://", "Engine/");
    filesystem.AddMount("game://", "Sandbox/");

    Application app;
    GServiceRegistry->RegisterService(&app);

    Renderer renderer;
    GServiceRegistry->RegisterService(&renderer);

    SystemManager systemManager;
    GServiceRegistry->RegisterService(&systemManager);

    WorldManager worldManager;
    GServiceRegistry->RegisterService(&worldManager);

    ControllerManager controllerManager;

#ifndef NDEBUG
    Editor editor;
    GServiceRegistry->RegisterService(&editor);

    RegisterEngineTypeDrawFuncs(editor);
#endif

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

#ifndef NDEBUG
    SDL3OpenGL editorBackend;
    editorBackend.Init();

    EditorWindowManager editorWindowManager;

    editorWindowManager.RegisterWindow<HierarchyEditorWindow>(editor, "Hierarchy");
    editorWindowManager.RegisterWindow<InspectorEditorWindow>(editor,"Inspector");

    editorWindowManager.Init();

#endif

    GameContext gameContext{.world = worldManager.GetActiveWorld()};

    controllerManager.RegisterController(MakeOwned<PlayerController>());

    controllerManager.InitControllers(gameContext);

    Sandbox sandbox;
    sandbox.Init();

    uint64_t previousTime = SDL_GetTicks();
    while (!app.ShouldClose())
    {
        app.Tick();

        const uint64_t currentTime = SDL_GetTicks();
        gameContext.deltaTime = static_cast<float>(currentTime - previousTime) / 1000.f;
        previousTime = currentTime;

        worldManager.GetActiveWorld().SwapEventBuffers();

        controllerManager.TickControllers(gameContext);
        systemManager.TickSystems(gameContext);

#ifndef NDEBUG
        editorBackend.BeginFrame();
        editorWindowManager.Tick();
        editorBackend.RenderFrame();
        editorBackend.EndFrame();
#endif

        renderer.Swap(app);
    }

    worldManager.ClearWorlds();

    GServiceRegistry->InvalidateService<WorldManager>();
    GServiceRegistry->InvalidateService<Renderer>();
    GServiceRegistry->InvalidateService<Application>();

    return 0;
}
