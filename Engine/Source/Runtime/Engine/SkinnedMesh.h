#pragma once
#include "Renderer/Renderer.h"

struct SkinJoint
{
    uint32 skeletonNode;
    glm::mat4 inverseBind{1.0f}; // Mesh-local to bone-local space at bind time.
};

// Shared runtime geometry and its mesh-specific binding to a skeleton.
struct SkinnedMesh
{
    MeshHandle mesh;
    List<SkinJoint> joints;
    glm::mat4 nodeTransform{1.0f};
};
