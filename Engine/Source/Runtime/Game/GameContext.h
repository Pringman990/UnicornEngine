#pragma once
#include "ECS/World.h"

struct GameContext
{
    World& world;
    f32 deltaTime{};
};
