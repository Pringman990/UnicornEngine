#include "pch.h"
#include "DebugDraw.h"

void DebugDraw::Clear()
{
    mLines.clear();
}

void DebugDraw::AddLine(glm::vec3 from, glm::vec3 to, glm::vec4 color)
{
    mLines.push_back({.from = from, .to = to, .color = color});
}

void DebugDraw::AddBox(glm::vec3 min, glm::vec3 max, glm::vec4 color)
{
    glm::vec3 p0 = {min.x, min.y, min.z};
    glm::vec3 p1 = {max.x, min.y, min.z};
    glm::vec3 p2 = {min.x, max.y, min.z};
    glm::vec3 p3 = {max.x, max.y, min.z};

    glm::vec3 p4 = {min.x, min.y, max.z};
    glm::vec3 p5 = {max.x, min.y, max.z};
    glm::vec3 p6 = {min.x, max.y, max.z};
    glm::vec3 p7 = {max.x, max.y, max.z};

    AddLine(p0, p1, color);
    AddLine(p0, p2, color);
    AddLine(p2, p3, color);
    AddLine(p3, p1, color);

    AddLine(p4, p5, color);
    AddLine(p4, p6, color);
    AddLine(p6, p7, color);
    AddLine(p7, p5, color);

    AddLine(p0, p4, color);
    AddLine(p1, p5, color);
    AddLine(p2, p6, color);
    AddLine(p3, p7, color);
}

void DebugDraw::AddRectangle(glm::vec3 center, glm::vec2 halfExtents, glm::vec3 normal, glm::vec4 color)
{
    glm::vec3 p0 = {center.x - halfExtents.x, center.y, center.z - halfExtents.y};
    glm::vec3 p1 = {center.x - halfExtents.x, center.y, center.z + halfExtents.y};
    glm::vec3 p2 = {center.x + halfExtents.x, center.y, center.z + halfExtents.y};
    glm::vec3 p3 = {center.x + halfExtents.x, center.y, center.z - halfExtents.y};

    AddLine(p0, p1, color);
    AddLine(p1, p2, color);
    AddLine(p2, p3, color);
    AddLine(p3, p0, color);
}

void DebugDraw::AddCircle(glm::vec3 center, glm::vec3 normal, f32 radius, glm::vec4 color)
{
}

void DebugDraw::AddSphere(glm::vec3 center, f32 radius, glm::vec4 color)
{
}

void DebugDraw::AddArrow(glm::vec3 from, glm::vec3 to, glm::vec4 color)
{
}
