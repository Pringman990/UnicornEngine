#include "pch.h"
#include "LayerStack.h"

void LayerStack::BeginFrame()
{
    for (auto* layer : mLayers)
        layer->BeginFrame();
}

Layer* LayerStack::MouseOwner() const
{
    for (auto* owner : mButtons)
        if (owner)
            return owner;
    return nullptr;
}

bool LayerStack::OnInput(const InputEvent& event)
{
    if (event.type == InputType::FocusLost)
    {
        ResetInput();
        return true;
    }

    Layer** owner = nullptr;
    if (event.key != Key::Unknown && (event.type == InputType::KeyDown || event.type == InputType::KeyUp))
        owner = &mKeys[static_cast<usize>(event.key)];
    if (event.button != MouseButton::None && (event.type == InputType::MouseDown || event.type == InputType::MouseUp))
        owner = &mButtons[static_cast<usize>(event.button)];

    // Deliver releases to whoever received the press, even underneath a new overlay.
    if (owner && *owner)
    {
        (*owner)->OnInput(event);
        if (event.type == InputType::KeyUp || event.type == InputType::MouseUp)
            *owner = nullptr;
        return true;
    }

    if (event.IsMouse())
    {
        if (auto* captured = MouseOwner())
        {
            captured->OnInput(event);
            if (owner && event.type == InputType::MouseDown)
                *owner = captured;
            return true;
        }
    }

    for (auto it = mLayers.rbegin(); it != mLayers.rend(); ++it)
    {
        if ((*it)->OnInput(event))
        {
            if (owner && (event.type == InputType::KeyDown || event.type == InputType::MouseDown))
                *owner = *it;
            return true;
        }
    }
    return false;
}

void LayerStack::ResetInput()
{
    mKeys.fill(nullptr);
    mButtons.fill(nullptr);
    for (auto* layer : mLayers)
        layer->ResetInput();
}

void LayerStack::Tick(f32 deltaTime)
{
    for (auto* layer : mLayers)
        layer->Tick(deltaTime);
}

void LayerStack::Render()
{
    for (auto* layer : mLayers)
        layer->Render();
}
