#include "pch.h"
#include "InputMapper.h"

void InputMapper::BindKey(ActionID action, Key key, glm::vec2 value)
{
    mBindings.push_back({action, InputType::KeyDown, key, MouseButton::None, value});
}

void InputMapper::BindButton(ActionID action, MouseButton button)
{
    mBindings.push_back({action, InputType::MouseDown, Key::Unknown, button});
}

void InputMapper::BindScroll(ActionID action)
{
    mBindings.push_back({action, InputType::Scroll});
    mValues.try_emplace(action, glm::vec2(0.0f));
}

void InputMapper::BindPointer(ActionID action)
{
    mBindings.push_back({action, InputType::MouseMove});
}

void InputMapper::OnAction(ActionID action, Func<bool(const ActionEvent&)> callback)
{
    mCallbacks[action] = std::move(callback);
}

void InputMapper::BeginFrame()
{
    mPointerAvailable = false;
    for (const auto& binding : mBindings)
        if (binding.type == InputType::Scroll)
            mValues[binding.action] = glm::vec2(0.0f);
}

bool InputMapper::Emit(ActionID action, ActionPhase phase, glm::vec2 value)
{
    const auto it = mCallbacks.find(action);
    return it == mCallbacks.end() || it->second({action, phase, value, mMousePosition});
}

bool InputMapper::OnInput(const InputEvent& event)
{
    if (event.IsMouse())
        mMousePosition = event.position;
    if (event.type == InputType::MouseMove)
        mPointerAvailable = true;

    bool handled = false;
    for (auto& binding : mBindings)
    {
        const bool key = binding.type == InputType::KeyDown && binding.key == event.key &&
                         (event.type == InputType::KeyDown || event.type == InputType::KeyUp);
        const bool button = binding.type == InputType::MouseDown && binding.button == event.button &&
                            (event.type == InputType::MouseDown || event.type == InputType::MouseUp);
        if (key || button)
        {
            // Repeats must not retrigger commands or add another held-key contribution.
            if (event.repeat)
                return binding.held;
            const bool down = event.type == InputType::KeyDown || event.type == InputType::MouseDown;
            if (binding.held == down)
                continue;
            const glm::vec2 previousValue = Value(binding.action);
            binding.held = down;
            glm::vec2 value(0.0f);
            for (const auto& other : mBindings)
                if (other.action == binding.action && other.held)
                    value += other.value;
            mValues[binding.action] = value;
            const bool accepted = Emit(binding.action, down ? ActionPhase::Pressed : ActionPhase::Released, value);
            if (down && !accepted)
            {
                binding.held = false;
                mValues[binding.action] = previousValue;
            }
            handled |= accepted;
        }
        else if (binding.type == event.type && event.type == InputType::Scroll)
        {
            mValues[binding.action] += event.delta;
            handled |= Emit(binding.action, ActionPhase::Changed, event.delta);
        }
        else if (binding.type == event.type && event.type == InputType::MouseMove)
        {
            handled |= Emit(binding.action, ActionPhase::Changed, event.position);
        }
    }
    return handled;
}

glm::vec2 InputMapper::Value(ActionID action) const
{
    const auto it = mValues.find(action);
    return it == mValues.end() ? glm::vec2(0.0f) : it->second;
}

void InputMapper::Reset()
{
    HashSet<ActionID> active;
    for (auto& binding : mBindings)
    {
        if (binding.held)
            active.insert(binding.action);
        binding.held = false;
    }
    for (auto& [action, value] : mValues)
        value = glm::vec2(0.0f);
    mPointerAvailable = false;
    for (ActionID action : active)
        Emit(action, ActionPhase::Canceled, glm::vec2(0.0f));
}

void InputMapper::Clear()
{
    Reset();
    mBindings.clear();
    mValues.clear();
    mCallbacks.clear();
}
