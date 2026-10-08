#pragma once
#include "ECS/World.h"
#include "Input/InputMapper.h"
#include "Renderer/Renderer.h"

struct GameContext
{
    World& world;
    InputMapper& input;
    f32 deltaTime{};
    RenderView renderView{};
};
