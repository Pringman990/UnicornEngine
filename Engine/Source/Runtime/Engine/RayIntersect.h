#pragma once

namespace ray
{
    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    Ray ScreenPointToRay(f32 mouseX, f32 mouseY, f32 width, f32 height, const glm::mat4& view, const glm::mat4& projection);

    bool RayPlaneIntersect(const Ray& ray, const glm::vec3& planePoint, const glm::vec3& planeNormal, glm::vec3& hit);
}
