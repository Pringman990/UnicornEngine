#pragma once
#include "Core/Types.h"
#include <glm/vec2.hpp>

// Engine keys are independent of the platform's key codes.
enum class Key
{
    Unknown,
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Space, Escape, Enter, Tab, Backspace,
    Left, Right, Up, Down,
    LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt,
    Count
};

enum class MouseButton { None, Left, Middle, Right, Count };

enum class InputType { KeyDown, KeyUp, MouseMove, MouseDown, MouseUp, Scroll, FocusLost };

struct InputEvent
{
    InputType type{};
    Key key = Key::Unknown;
    MouseButton button = MouseButton::None;
    glm::vec2 position{}; // Window coordinates, starting at the top left.
    glm::vec2 delta{};
    bool repeat = false;

    bool IsMouse() const
    {
        return type == InputType::MouseMove || type == InputType::MouseDown ||
               type == InputType::MouseUp || type == InputType::Scroll;
    }
};
