#pragma once
#include "Skeleton.h"
#include "SkinnedMesh.h"
#include "AnimationClip.h"

struct MeshImportData
{
    struct Mesh
    {
        struct SubMesh
        {
            uint32 startIndex;
            uint32 indexCount;
            String materialName;
        };

        List<SubMesh> subMeshes;
        List<Vertex> vertices;
        List<uint32> indices;

        // Empty for static meshes; otherwise one entry per geometry vertex.
        // Vertex influences index skinJoints; joints index Skeleton::nodes.
        List<SkinVertex> skinVertices;
        List<SkinJoint> skinJoints;
        glm::mat4 nodeTransform{1.0f}; // Mesh-local to imported model space.
    };

    List<Mesh> meshes;
    // Created for skinned geometry or animation when no existing skeleton was supplied.
    Optional<Skeleton> skeleton;
    List<AnimationClip> animations;
};

class MeshImporter
{
public:
    // hint is the source format extension, for example "fbx".
    // Imports geometry, its skeleton, and every clip present in the file.
    // Animation-only files can use a supplied skeleton. Channels must match its hierarchy;
    // skinned geometry must also match its default transforms. No retargeting is performed.
    static Expected<MeshImportData, String> Import(
        const ByteBuffer& data,
        const String& hint,
        const Skeleton* existingSkeleton = nullptr);
};
