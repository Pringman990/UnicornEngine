#pragma once
#include "Input/InputEvent.h"

class Layer
{
public:
    virtual ~Layer() = default;
    virtual void BeginFrame() {}
    virtual bool OnInput(const InputEvent& event) { return false; }
    virtual void ResetInput() {}
    virtual void Tick(f32 deltaTime) {}
    virtual void Render() {}
};
