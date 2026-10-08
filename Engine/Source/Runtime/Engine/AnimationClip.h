#pragma once
#include "Core/Types.h"
#include <glm/gtc/quaternion.hpp>

struct AnimationClip
{
    template<typename T>
    struct Key
    {
        float time; // Seconds.
        T value;
    };

    struct Track
    {
        uint32 node;
        List<Key<glm::vec3>> positions;
        List<Key<glm::quat>> rotations;
        List<Key<glm::vec3>> scales;
    };

    float duration = 0.0f;
    List<Track> tracks;
};
