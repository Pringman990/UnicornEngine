#pragma once
#include "Core/Types.h"
#include "glm/vec3.hpp"

struct Movement
{
    f32 maxSpeed = 5.0f;
    glm::vec3 velocity = glm::vec3(0.0f);
};

struct MoveTarget
{
    bool active = false;
    glm::vec3 position = glm::vec3(0.0f);
    f32 stoppingDistance = 0.1f;
};