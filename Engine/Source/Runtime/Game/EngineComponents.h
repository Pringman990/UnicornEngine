#pragma once

#include "Renderer/Renderer.h"
#include "Engine/AnimationClip.h"
#include "Engine/Skeleton.h"
#include "Engine/SkinnedMesh.h"

struct Transform
{
    glm::vec3 position{0};
    glm::quat rotation{1, 0, 0, 0};
    glm::vec3 scale{1.0f};

    glm::mat4 world = glm::mat4(1.0f);
};

inline glm::mat4x4 GetMatrix(const Transform& T)
{
    return glm::translate(glm::mat4(1.0f), T.position) *
        glm::mat4_cast(T.rotation) *
        glm::scale(glm::mat4(1.0f), T.scale);
}

struct Camera
{
    float fov = glm::radians(60.0f);
    float aspect = 16.0f / 9.0f;

    float nearPlane = 0.01f;
    float farPlane = 1000.0f;
};

struct CharacterController
{
    float moveSpeed = 3.0f;
    float stepHeight = 0.15f;
    glm::vec2 velocity{0.0f, 0.0f};
};

struct Model
{
    List<MeshHandle> meshes;
    List<MaterialHandle> materials;
};

struct MeshRenderer
{
    Model model{};
};

struct Animator
{
    const Skeleton* skeleton = nullptr;
    const AnimationClip* clip = nullptr;
    float currentTime = 0.0f;
    bool loop = true; // Non-looping clips hold their final pose.
    List<glm::mat4> pose;
};

struct AnimatedMeshRenderer
{
    const SkinnedMesh* mesh = nullptr;
    MaterialHandle material;
    List<glm::mat4> jointMatrices;
};

struct SpriteRenderer
{
    MaterialHandle material;
    TextureHandle texture;

    glm::vec4 rect{};

    float pixelsPerUnit = 100.0f;
    glm::vec2 pivot{0.5f, 0.0f};
};

struct AnimationSequence2D
{
    List<glm::vec4> rects;
    float frameDuration = 0.12f;
};

struct Animator2D
{
    AnimationSequence2D* sequence{};
    float currentTime = 0.0f;
    uint32 currentFrame = 0;
};

struct BoxCollider2D
{
    glm::vec2 offset{};
    glm::vec2 halfExtents{};

    bool walkable = false;
};

void RegisterEngineComponents();
