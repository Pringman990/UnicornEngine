#pragma once
#include "Core/ServiceRegistry.h"

struct DebugLine
{
    glm::vec3 from;
    glm::vec3 to;
    glm::vec4 color;
};

class DebugDraw
{
    INIT_SERVICE(DebugDraw);

public:
    void Clear();

    void AddLine(glm::vec3 from, glm::vec3 to, glm::vec4 color);
    void AddBox(glm::vec3 min, glm::vec3 max, glm::vec4 color);
    void AddRectangle(glm::vec3 center, glm::vec2 halfExtents, glm::vec3 normal, glm::vec4 color);
    void AddCircle(glm::vec3 center, glm::vec3 normal, f32 radius, glm::vec4 color);
    void AddSphere(glm::vec3 center, f32 radius, glm::vec4 color);
    void AddArrow(glm::vec3 from, glm::vec3 to, glm::vec4 color);

    const auto& GetLines() const { return mLines; }

private:
    List<DebugLine> mLines;
};
