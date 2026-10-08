#pragma once
#include "Engine/Layer.h"
#include "Backend/Sdl3OpenGl.h"
#include "EditorWindowManager.h"

class EditorLayer : public Layer
{
public:
    EditorLayer(SDL3OpenGL& backend, EditorWindowManager& windows) : mBackend(backend), mWindows(windows) {}
    void BeginFrame() override;
    bool OnInput(const InputEvent& event) override;
    void Render() override;

private:
    SDL3OpenGL& mBackend;
    EditorWindowManager& mWindows;
};
