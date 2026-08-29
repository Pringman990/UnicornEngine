#pragma once

struct AABB
{
    glm::vec2 min;
    glm::vec2 max;
};

struct CollisionResult
{
    glm::vec2 normal;
    float penetration;
};

class DetectionTest
{
public:
    static bool Overlaps(const AABB& a, const AABB& b);

    static bool GetCollision(const AABB& a, const AABB& b, CollisionResult& collision);

};