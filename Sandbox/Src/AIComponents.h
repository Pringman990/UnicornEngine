#pragma once
#include <limits>
#include "Core/Types.h"
#include "Engine/AnimationClip.h"
#include "ECS/EntityManager.h"
#include "glm/vec3.hpp"

struct Team
{
    enum Id { Player, Enemy };
    Id id = Player;
};

struct Health
{
    f32 current = 100.0f;
    f32 maximum = 100.0f;
};

struct Attack
{
    Entity target = InvalidEntity;
    f32 range = 2.0f; // Distance between unit centres on the XZ plane.
    f32 damage = 25.0f;
    f32 interval = 1.0f;
    f32 cooldown = 0.0f;
};

struct Movement
{
    f32 maxSpeed = 5.0f;
    f32 radius = 0.4f; // Collision circle on the XZ plane.
    glm::vec3 velocity = glm::vec3(0.0f);
};

struct MoveTarget
{
    bool active = false;
    glm::vec3 position = glm::vec3(0.0f);
    f32 stoppingDistance = 0.1f;
    f32 closestDistance = std::numeric_limits<f32>::max();
    f32 timeWithoutProgress = 0.0f;
};

struct UnitAnimations
{
    const AnimationClip* idle = nullptr;
    const AnimationClip* run = nullptr;
    const AnimationClip* attack = nullptr;
};
