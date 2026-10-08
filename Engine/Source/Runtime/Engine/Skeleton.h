#pragma once

#include "Core/Types.h"
#include <glm/glm.hpp>

struct Skeleton
{
    struct Node
    {
        String name;
        int32 parent = -1;
        glm::mat4 defaultLocal{1.0f};
    };

    // Parents precede their children. Includes transform helpers, not just weighted bones.
    List<Node> nodes;
};
