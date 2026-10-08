#pragma once
#include "Input/InputMapper.h"

// The engine maps action IDs; the game decides what those actions mean.
namespace GameActions
{
    enum : ActionID { CameraMove, CameraZoom, Select, Pointer, Command, Stop };
}
