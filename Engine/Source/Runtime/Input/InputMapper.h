#pragma once
#include "InputEvent.h"

using ActionID = uint32;
enum class ActionPhase { Pressed, Released, Changed, Canceled };

struct ActionEvent
{
    ActionID action;
    ActionPhase phase;
    glm::vec2 value;
    glm::vec2 position;
};

class InputMapper
{
public:
    void BindKey(ActionID action, Key key, glm::vec2 value = {1.0f, 0.0f});
    void BindButton(ActionID action, MouseButton button);
    void BindScroll(ActionID action);
    void BindPointer(ActionID action);
    void OnAction(ActionID action, Func<bool(const ActionEvent&)> callback);

    void BeginFrame();
    bool OnInput(const InputEvent& event);
    void Reset();
    void Clear();
    glm::vec2 Value(ActionID action) const;
    glm::vec2 MousePosition() const { return mMousePosition; }
    bool PointerAvailable() const { return mPointerAvailable; }

private:
    struct Binding
    {
        ActionID action;
        InputType type;
        Key key = Key::Unknown;
        MouseButton button = MouseButton::None;
        glm::vec2 value{1.0f, 0.0f};
        bool held = false;
    };
    bool Emit(ActionID action, ActionPhase phase, glm::vec2 value);

    List<Binding> mBindings;
    UnorderedMap<ActionID, glm::vec2> mValues;
    UnorderedMap<ActionID, Func<bool(const ActionEvent&)>> mCallbacks;
    glm::vec2 mMousePosition{};
    bool mPointerAvailable = false;
};
