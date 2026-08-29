//
// Created on 2026-08-11.
//
#include "pch.h"
#include "DetectionTest.h"

bool DetectionTest::Overlaps(const AABB& a, const AABB& b)
{
    return a.min.x < b.max.x &&
           a.max.x > b.min.x &&
           a.min.y < b.max.y &&
           a.max.y > b.min.y;
}

bool DetectionTest::GetCollision(const AABB& a, const AABB& b, CollisionResult& collision)
{
    if (!Overlaps(a, b))
        return false;

    float overlapX = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    float overlapY = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);

    glm::vec2 centerA = (a.min + a.max) * 0.5f;
    glm::vec2 centerB = (b.min + b.max) * 0.5f;

    glm::vec2 direction = centerA - centerB;

    if (overlapX < overlapY)
    {
        collision.penetration = overlapX;

        collision.normal = glm::vec2(direction.x < 0.0f ? -1.0f : 1.0f, 0.0f);
    }
    else
    {
        collision.penetration = overlapY;

        collision.normal = glm::vec2(0.0f, direction.y < 0.0f ? -1.0f : 1.0f);
    }

    return true;
}
