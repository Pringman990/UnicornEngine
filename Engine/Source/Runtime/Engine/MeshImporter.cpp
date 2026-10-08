#include "pch.h"
#include "MeshImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <execution>
#include <limits>

namespace
{
    // Temporary map from Assimp nodes to indices in the new or supplied skeleton.
    using NodeIndices = UnorderedMap<const aiNode*, uint32>;
    constexpr uint32 InvalidNode = std::numeric_limits<uint32>::max();

    glm::mat4 ToMatrix(const aiMatrix4x4& matrix)
    {
        // Assimp names elements by row; GLM's constructor takes columns.
        return glm::mat4(
            matrix.a1, matrix.b1, matrix.c1, matrix.d1,
            matrix.a2, matrix.b2, matrix.c2, matrix.d2,
            matrix.a3, matrix.b3, matrix.c3, matrix.d3,
            matrix.a4, matrix.b4, matrix.c4, matrix.d4);
    }

    bool SameTransform(const glm::mat4& left, const glm::mat4& right)
    {
        // Allow small export rounding differences when checking skeleton compatibility.
        for (uint32 column = 0; column < 4; ++column)
        {
            for (uint32 row = 0; row < 4; ++row)
            {
                const f32 a = left[column][row];
                const f32 b = right[column][row];
                if (!std::isfinite(a) || !std::isfinite(b) ||
                    std::abs(a - b) > 0.0001f * std::max({1.0f, std::abs(a), std::abs(b)}))
                    return false;
            }
        }
        return true;
    }

    void ImportSkeleton(const aiNode* node, int32 parent, Skeleton& skeleton, NodeIndices& indices)
    {
        // Store parents first so global transforms can later be evaluated in one pass.
        // Keep helper nodes too: they can carry transforms between weighted bones.
        const auto index = static_cast<uint32>(skeleton.nodes.size());
        skeleton.nodes.push_back({.name = node->mName.C_Str(), .parent = parent, .defaultLocal = ToMatrix(node->mTransformation)});
        indices.emplace(node, index);

        for (uint32 i = 0; i < node->mNumChildren; ++i)
            ImportSkeleton(node->mChildren[i], static_cast<int32>(index), skeleton, indices);
    }

    Expected<uint32, String> ResolveNode(
        const aiNode* node, const Skeleton& skeleton,
        const UnorderedMap<String, uint32>& names, NodeIndices& indices,
        bool matchDefaultTransform = true)
    {
        if (const auto it = indices.find(node); it != indices.end())
            return it->second;

        const auto it = names.find(node->mName.C_Str());
        if (it == names.end() || it->second == InvalidNode)
            return Unexpected(fmt::format("Skeleton node '{}' is missing or ambiguous", node->mName.C_Str()));

        const uint32 index = it->second;
        const auto& target = skeleton.nodes[index];
        int32 parent = -1;
        // Matching bone names alone is insufficient. Match the ancestor chain as well.
        if (node->mParent)
        {
            auto resolved = ResolveNode(node->mParent, skeleton, names, indices, matchDefaultTransform);
            if (!resolved)
                return Unexpected(resolved.error());
            parent = static_cast<int32>(*resolved);
        }

        // Geometry reuse requires matching defaults. Clips may be exported in a different pose.
        if (target.parent != parent ||
            (matchDefaultTransform && !SameTransform(target.defaultLocal, ToMatrix(node->mTransformation))))
            return Unexpected(fmt::format("Skeleton node '{}' has an incompatible hierarchy or default transform", target.name));

        indices.emplace(node, index);
        return index;
    }

    Expected<void, String> ImportAnimations(
        const aiScene* scene, const Skeleton& skeleton,
        const UnorderedMap<String, uint32>& names, NodeIndices& nodes,
        List<AnimationClip>& clips)
    {
        clips.reserve(scene->mNumAnimations);
        for (uint32 animation = 0; animation < scene->mNumAnimations; ++animation)
        {
            const aiAnimation& source = *scene->mAnimations[animation];
            if (!std::isfinite(source.mTicksPerSecond) || source.mTicksPerSecond <= 0.0 ||
                !std::isfinite(source.mDuration) || source.mDuration <= 0.0)
                return Unexpected(String("Animation needs a positive duration and tick rate"));
            if (!source.mNumChannels)
                return Unexpected(String("Animation has no node transform channels"));

            AnimationClip clip;
            clip.duration = static_cast<float>(source.mDuration / source.mTicksPerSecond);
            clip.tracks.reserve(source.mNumChannels);
            for (uint32 i = 0; i < source.mNumChannels; ++i)
            {
                const aiNodeAnim& channel = *source.mChannels[i];
                const aiNode* node = scene->mRootNode->FindNode(channel.mNodeName);
                if (!node)
                    return Unexpected(String("Animation node missing from imported hierarchy: ") + channel.mNodeName.C_Str());

                // Map channels once, including FBX helper nodes. Match the same rig, not its pose.
                auto resolved = ResolveNode(node, skeleton, names, nodes, false);
                if (!resolved)
                    return Unexpected(resolved.error());
                if (!channel.mNumPositionKeys || !channel.mNumRotationKeys || !channel.mNumScalingKeys)
                    return Unexpected(String("Animation channel has missing transform keys"));

                AnimationClip::Track track;
                track.node = *resolved;
                for (uint32 key = 0; key < channel.mNumPositionKeys; ++key)
                {
                    const auto& k = channel.mPositionKeys[key];
                    track.positions.push_back({static_cast<float>(k.mTime / source.mTicksPerSecond),
                        {k.mValue.x, k.mValue.y, k.mValue.z}});
                }
                for (uint32 key = 0; key < channel.mNumRotationKeys; ++key)
                {
                    const auto& k = channel.mRotationKeys[key];
                    track.rotations.push_back({static_cast<float>(k.mTime / source.mTicksPerSecond),
                        glm::normalize(glm::quat(k.mValue.w, k.mValue.x, k.mValue.y, k.mValue.z))});
                }
                for (uint32 key = 0; key < channel.mNumScalingKeys; ++key)
                {
                    const auto& k = channel.mScalingKeys[key];
                    track.scales.push_back({static_cast<float>(k.mTime / source.mTicksPerSecond),
                        {k.mValue.x, k.mValue.y, k.mValue.z}});
                }
                clip.tracks.push_back(std::move(track));
            }
            clips.push_back(std::move(clip));
        }
        return {};
    }

    Expected<void, String> ImportSkin(
        const aiMesh* source, const aiScene* scene,
        const NodeIndices& nodes, MeshImportData::Mesh& mesh)
    {
        mesh.skinVertices.resize(source->mNumVertices);
        mesh.skinJoints.reserve(source->mNumBones);

        for (uint32 joint = 0; joint < source->mNumBones; ++joint)
        {
            const aiBone& bone = *source->mBones[joint];
            const aiNode* node = scene->mRootNode->FindNode(bone.mName);
            const auto it = nodes.find(node);
            if (it == nodes.end())
                return Unexpected(fmt::format("Bone '{}' has no skeleton node", bone.mName.C_Str()));

            // Copy the exported mesh-to-bone bind matrix directly. The hierarchy's
            // default pose is not necessarily the bind pose, so do not derive it there.
            mesh.skinJoints.push_back({it->second, ToMatrix(bone.mOffsetMatrix)});

            // Assimp stores weights per bone. Convert them to influences per vertex.
            for (uint32 i = 0; i < bone.mNumWeights; ++i)
            {
                const aiVertexWeight& influence = bone.mWeights[i];
                if (influence.mVertexId >= source->mNumVertices ||
                    !std::isfinite(influence.mWeight) || influence.mWeight < 0.0f)
                    return Unexpected(fmt::format("Bone '{}' has an invalid vertex weight", bone.mName.C_Str()));

                auto& vertex = mesh.skinVertices[influence.mVertexId];
                // Retain the four strongest influences by replacing the weakest slot.
                uint32 smallest = 0;
                for (uint32 slot = 1; slot < 4; ++slot)
                {
                    if (vertex.weights[slot] < vertex.weights[smallest])
                        smallest = slot;
                }

                if (influence.mWeight > vertex.weights[smallest])
                {
                    vertex.jointIndices[smallest] = joint;
                    vertex.weights[smallest] = influence.mWeight;
                }
            }
        }

        // Dropping weaker influences changes the total; normalize the retained weights.
        for (uint32 i = 0; i < mesh.skinVertices.size(); ++i)
        {
            auto& vertex = mesh.skinVertices[i];
            const float total = vertex.weights.x + vertex.weights.y + vertex.weights.z + vertex.weights.w;
            if (!std::isfinite(total) || total <= 0.0f)
                return Unexpected(fmt::format("Skinned vertex {} has no usable weights", i));
            vertex.weights /= total;
        }
        return {};
    }

    Expected<void, String> ImportAiMesh(const aiScene* scene, const aiMesh* source, MeshImportData::Mesh& mesh)
    {
        // Several static source meshes may share a destination. Offset their indices
        // and retain each source material as a separate submesh range.
        const uint32 vertexOffset = static_cast<uint32>(mesh.vertices.size());
        const uint32 indexOffset = static_cast<uint32>(mesh.indices.size());

        for (uint32 i = 0; i < source->mNumVertices; ++i)
        {
            Vertex vertex;
            const auto& position = source->mVertices[i];
            vertex.position = glm::vec3(position.x, position.y, position.z);

            if (source->HasNormals())
            {
                const auto& normal = source->mNormals[i];
                vertex.normal = glm::vec3(normal.x, normal.y, normal.z);
            }
            if (source->mTextureCoords[0])
            {
                const auto& uv = source->mTextureCoords[0][i];
                vertex.uv = glm::vec2(uv.x, uv.y);
            }
            if (source->HasTangentsAndBitangents())
            {
                const auto& tangent = source->mTangents[i];
                const auto& bitangent = source->mBitangents[i];
                vertex.tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
                vertex.bitangent = glm::vec3(bitangent.x, bitangent.y, bitangent.z);
            }
            mesh.vertices.push_back(vertex);
        }

        for (uint32 i = 0; i < source->mNumFaces; ++i)
        {
            const aiFace& face = source->mFaces[i];
            if (face.mNumIndices != 3)
                return Unexpected(String("Mesh contains a non-triangle face"));
            for (uint32 index = 0; index < 3; ++index)
            {
                if (face.mIndices[index] >= source->mNumVertices)
                    return Unexpected(String("Mesh contains an invalid vertex index"));
            }
        }

        mesh.indices.resize(indexOffset + source->mNumFaces * 3);
        // Each face writes its own three slots, so parallel writes do not overlap.
        std::for_each(std::execution::par, source->mFaces, source->mFaces + source->mNumFaces,
            [&](const aiFace& face)
            {
                const size_t offset = indexOffset + (&face - source->mFaces) * 3;
                for (uint32 index = 0; index < 3; ++index)
                    mesh.indices[offset + index] = face.mIndices[index] + vertexOffset;
            });

        if (source->mMaterialIndex >= scene->mNumMaterials)
            return Unexpected(String("Mesh references an invalid material"));

        mesh.subMeshes.push_back({indexOffset, static_cast<uint32>(mesh.indices.size() - indexOffset),
            scene->mMaterials[source->mMaterialIndex]->GetName().C_Str()});
        return {};
    }

    Expected<void, String> ImportMeshes(
        MeshImportData& data, const aiScene* scene, const aiNode* node,
        const glm::mat4& parentTransform, const NodeIndices& nodes)
    {
        // Keep vertices in mesh-local space and record the full node-to-model transform.
        const glm::mat4 transform = parentTransform * ToMatrix(node->mTransformation);
        MeshImportData::Mesh staticMesh;
        staticMesh.nodeTransform = transform;

        for (uint32 i = 0; i < node->mNumMeshes; ++i)
        {
            const aiMesh* source = scene->mMeshes[node->mMeshes[i]];
            if (source->HasBones())
            {
                // Each skinned source keeps its own joint list and inverse bind matrices.
                MeshImportData::Mesh mesh;
                mesh.nodeTransform = transform;
                auto geometry = ImportAiMesh(scene, source, mesh);
                if (!geometry)
                    return Unexpected(geometry.error());
                auto skin = ImportSkin(source, scene, nodes, mesh);
                if (!skin)
                    return Unexpected(skin.error());
                data.meshes.push_back(std::move(mesh));
            }
            else
            {
                // Static meshes on the same node share a transform and can be combined.
                auto geometry = ImportAiMesh(scene, source, staticMesh);
                if (!geometry)
                    return Unexpected(geometry.error());
            }
        }

        if (!staticMesh.subMeshes.empty())
            data.meshes.push_back(std::move(staticMesh));

        for (uint32 i = 0; i < node->mNumChildren; ++i)
        {
            auto child = ImportMeshes(data, scene, node->mChildren[i], transform, nodes);
            if (!child)
                return Unexpected(child.error());
        }
        return {};
    }
}

Expected<MeshImportData, String> MeshImporter::Import(
    const ByteBuffer& data, const String& hint, const Skeleton* existingSkeleton)
{
    // Assimp owns the scene until importer is destroyed. Return only copied engine data.
    Assimp::Importer importer;
    constexpr uint32 readFlags =
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs |
        aiProcess_GlobalScale |
        aiProcess_ValidateDataStructure;

    const aiScene* scene = importer.ReadFileFromMemory(data.data(), data.size(), readFlags, hint.c_str());
    if (!scene || !scene->mRootNode)
    {
        String error = importer.GetErrorString();
        LOG_WARNING("Can't load assimp file, error {}", error);
        return Unexpected(error);
    }
    // Animation-only files may be marked incomplete because they have no geometry.
    if (scene->mNumMeshes && (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        return Unexpected(String("Mesh file contains incomplete geometry"));
    if (!scene->mNumMeshes && !scene->mNumAnimations)
        return Unexpected(String("File contains no meshes or animations"));

    const bool skinned = scene->mNumMeshes && std::any_of(scene->mMeshes, scene->mMeshes + scene->mNumMeshes,
        [](const aiMesh* mesh) { return mesh->HasBones(); });
    if (existingSkeleton && !skinned && !scene->mNumAnimations)
        return Unexpected(String("Mesh file has no skin binding for the supplied skeleton"));

    MeshImportData result;
    NodeIndices nodes;
    UnorderedMap<String, uint32> names;
    if ((skinned || scene->mNumAnimations) && !existingSkeleton)
    {
        // Flatten the imported hierarchy into a new skeleton and record its node indices.
        result.skeleton.emplace();
        ImportSkeleton(scene->mRootNode, -1, *result.skeleton, nodes);
    }
    else if (existingSkeleton)
    {
        // Resolve names against the supplied skeleton; its indices may differ from the file.
        for (uint32 i = 0; i < existingSkeleton->nodes.size(); ++i)
        {
            const auto& node = existingSkeleton->nodes[i];
            if (node.parent < -1 || node.parent >= static_cast<int32>(i))
                return Unexpected(String("Skeleton parents must precede their children"));
            const auto [it, inserted] = names.emplace(node.name, i);
            // Duplicate names cannot identify a node reliably if a bone needs that name.
            if (!inserted)
                it->second = InvalidNode;
        }

        // Only weighted bones and their ancestors must exist in the supplied skeleton.
        // Mesh attachment nodes can differ between files using the same rig.
        for (uint32 mesh = 0; mesh < scene->mNumMeshes; ++mesh)
        {
            const auto* source = scene->mMeshes[mesh];
            for (uint32 bone = 0; bone < source->mNumBones; ++bone)
            {
                const aiNode* node = scene->mRootNode->FindNode(source->mBones[bone]->mName);
                if (!node)
                    return Unexpected(String("Mesh bone has no node in the imported hierarchy"));
                auto resolved = ResolveNode(node, *existingSkeleton, names, nodes);
                if (!resolved)
                    return Unexpected(resolved.error());
            }
        }
    }

    auto meshes = ImportMeshes(result, scene, scene->mRootNode, glm::mat4(1.0f), nodes);
    if (!meshes)
        return Unexpected(meshes.error());

    if (scene->mNumAnimations)
    {
        const Skeleton& skeleton = existingSkeleton ? *existingSkeleton : *result.skeleton;
        auto animations = ImportAnimations(scene, skeleton, names, nodes, result.animations);
        if (!animations)
            return Unexpected(animations.error());
    }
    return result;
}
