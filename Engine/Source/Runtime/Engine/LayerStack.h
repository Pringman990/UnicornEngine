#pragma once
#include "Layer.h"

class LayerStack
{
public:
    // Borrowed layers must outlive the stack. Add from bottom to top.
    void Add(Layer& layer) { mLayers.push_back(&layer); }
    void BeginFrame();
    bool OnInput(const InputEvent& event);
    void ResetInput();
    void Tick(f32 deltaTime);
    void Render();

private:
    Layer* MouseOwner() const;
    List<Layer*> mLayers;
    Array<Layer*, static_cast<usize>(Key::Count)> mKeys{};
    Array<Layer*, static_cast<usize>(MouseButton::Count)> mButtons{};
};
