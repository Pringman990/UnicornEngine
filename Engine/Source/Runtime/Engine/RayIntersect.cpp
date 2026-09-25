#include "pch.h"
#include "RayIntersect.h"

ray::Ray ray::ScreenPointToRay(f32 mouseX, f32 mouseY, f32 width, f32 height, const glm::mat4& view, const glm::mat4& projection)
{
    f32 ndcX = 2.0f * mouseX / width - 1.0f;
    f32 ndcY = 1.0f - 2.0f * mouseY / height;

    glm::mat4 invVP = glm::inverse(projection * view);

    glm::vec4 near = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 far = invVP * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

    near /= near.w;
    far /= far.w;

    Ray ray{};
    ray.origin = glm::vec3(near);
    ray.direction = glm::normalize(glm::vec3(far - near));
    return ray;
}

bool ray::RayPlaneIntersect(const Ray& ray, const glm::vec3& planePoint, const glm::vec3& planeNormal, glm::vec3& hit)
{
    f32 denom = glm::dot(ray.direction, planeNormal);

    if (glm::abs(denom) < 0.000001f)
        return false;

    f32 t = glm::dot(planePoint - ray.origin, planeNormal) / denom;

    if (t < 0.0f)
        return false;

    hit = ray.origin + ray.direction * t;
    return true;
}
