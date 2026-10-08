#pragma once
#include "InputEvent.h"

// The application feeds translated events here. Consumers never need SDL types.
class InputSystem
{
public:
    void BeginFrame() { mEvents.clear(); }
    void Push(const InputEvent& event) { mEvents.push_back(event); }
    Span<const InputEvent> GetEvents() const { return mEvents; }

private:
    List<InputEvent> mEvents;
};
