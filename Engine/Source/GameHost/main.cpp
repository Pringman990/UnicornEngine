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
#include "Engine/LayerStack.h"
#include "UI/UI.h"
#include "UI/UILayer.h"
#include "Game/GameplayLayer.h"

#ifndef NDEBUG
#include <Editor.h>
#include <Backend/Sdl3OpenGl.h>
#include <EditorWindowManager.h>
#include <EditorLayer.h>
#include <Windows/HierarchyEditorWindow.h>
#include <Windows/InspectorEditorWindow.h>
#include "EngineTypeDrawFuncs.h"
#endif

#include "../Runtime/Game/EngineComponents.h"
#include "Core/ReflectionRegistry.h"

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

    UI ui;
    GServiceRegistry->RegisterService(&ui);

    SystemManager systemManager;
    GServiceRegistry->RegisterService(&systemManager);

    WorldManager worldManager;
    GServiceRegistry->RegisterService(&worldManager);

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
    ui.Init();

#ifndef NDEBUG
    SDL3OpenGL editorBackend;
    editorBackend.Init();

    EditorWindowManager editorWindowManager;

    editorWindowManager.RegisterWindow<HierarchyEditorWindow>(editor, "Hierarchy");
    editorWindowManager.RegisterWindow<InspectorEditorWindow>(editor,"Inspector");

    editorWindowManager.Init();

#endif

    GameplayLayer gameplay(worldManager.GetActiveWorld(), systemManager);
    gameplay.RegisterController(MakeOwned<PlayerController>());
    gameplay.Init();

    Sandbox sandbox;
    sandbox.Init();

    UILayer uiLayer;
#ifndef NDEBUG
    EditorLayer editorLayer(editorBackend, editorWindowManager);
#endif
    LayerStack layers;
    layers.Add(gameplay);
    layers.Add(uiLayer);
#ifndef NDEBUG
    layers.Add(editorLayer);
#endif

    uint64_t previousTime = SDL_GetTicks();
    while (!app.ShouldClose())
    {
        app.Tick();

        const uint64_t currentTime = SDL_GetTicks();
        const f32 deltaTime = static_cast<f32>(currentTime - previousTime) / 1000.f;
        previousTime = currentTime;

        layers.BeginFrame();
        for (const auto& event : app.GetInput().GetEvents())
            layers.OnInput(event);
        layers.Tick(deltaTime);
        layers.Render();

        renderer.Swap(app);
    }

    layers.ResetInput();
    gameplay.Destroy();
    worldManager.ClearWorlds();

    ui.Destroy();
    GServiceRegistry->InvalidateService<UI>();

    GServiceRegistry->InvalidateService<WorldManager>();
    GServiceRegistry->InvalidateService<Renderer>();
    GServiceRegistry->InvalidateService<Application>();

    return 0;
}
