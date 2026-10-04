#include "pch.h"
#include "MeshImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <execution>

Expected<MeshImportData, String> MeshImporter::Import(const ByteBuffer& data, const String& hint)
{
    Assimp::Importer importer{};

    constexpr uint32 readFlags =
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs |
        aiProcess_GlobalScale;

    const aiScene* scene = importer.ReadFileFromMemory(data.data(), data.size(), readFlags, hint.c_str());
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        String error = importer.GetErrorString();
        LOG_WARNING("Can't load assimp file, error {}", error);
        return Unexpected(error);
    }

    MeshImportData importData{};
    ImportMeshes(importData, scene, scene->mRootNode);

    return importData;
}

void MeshImporter::ImportMeshes(MeshImportData& importData, const aiScene* scene, const aiNode* node)
{
    if (node->mNumMeshes > 0)
    {
        MeshImportData::Mesh mesh;
        uint32 indexOffset = 0;
        uint32 vertexOffset = 0;
        std::vector<Vertex> vertices;
        std::vector<uint32> indices;

        mesh.subMeshes.resize(node->mNumMeshes);

        //This represent ONE Mesh
        for (uint32 i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
            mesh.subMeshes[i] = ImportAiMesh(importData, scene, aiMesh, indexOffset, vertexOffset, indices, vertices, mesh);
        }

        mesh.indices = indices;
        mesh.vertices = vertices;

        importData.meshes.push_back(mesh);
    }

    //These are new meshes for rendering but in the hierarchy they are children
    for (uint32 i = 0; i < node->mNumChildren; i++)
    {
        ImportMeshes(importData, scene, node->mChildren[i]);
    }
}

MeshImportData::Mesh::SubMesh MeshImporter::ImportAiMesh(
    MeshImportData& ImportData,
    const aiScene* AiScene,
    aiMesh* AiMesh, uint32& IndexOffset,
    uint32& VertexOffset, List<uint32>& Indices,
    List<Vertex>& Vertices,
    MeshImportData::Mesh& mesh
)
{
    for (uint32 i = 0; i < AiMesh->mNumVertices; i++)
    {
        Vertex vertex;
        aiVector3D aiVertex = AiMesh->mVertices[i];

        vertex.position = glm::vec4(aiVertex.x, aiVertex.y, aiVertex.z, 1);

        if (AiMesh->HasNormals())
        {
            aiVector3D aiNormals = AiMesh->mNormals[i];
            vertex.normal = glm::vec3(aiNormals.x, aiNormals.y, aiNormals.z);
        }

        if (AiMesh->mTextureCoords[0])
        {
            aiVector3D aiUV = AiMesh->mTextureCoords[0][i];
            vertex.uv = glm::vec2(aiUV.x, aiUV.y);
        }

        if (AiMesh->HasTangentsAndBitangents())
        {
            aiVector3D aiTangent = AiMesh->mTangents[i];
            aiVector3D aiBiTangent = AiMesh->mBitangents[i];
            vertex.tangent = glm::vec3(aiTangent.x, aiTangent.y, aiTangent.z);
            vertex.bitangent = glm::vec3(aiBiTangent.x, aiBiTangent.y, aiBiTangent.z);
        }

        Vertices.push_back(vertex);
    }

    //We know it will always be 3 indices per face as we us aiProcess_Triangulate when importing the scene
    size_t size = Indices.size();
    Indices.resize((size + (AiMesh->mNumFaces * 3)));

    std::for_each(std::execution::par, AiMesh->mFaces, AiMesh->mFaces + AiMesh->mNumFaces,
                  [&](const aiFace& face)
                  {
                      const size_t faceIndex = &face - AiMesh->mFaces;
                      const size_t globalOffset = size + (faceIndex * 3);

                      Indices[globalOffset] = face.mIndices[0] + VertexOffset;
                      Indices[globalOffset + 1] = face.mIndices[1] + VertexOffset;
                      Indices[globalOffset + 2] = face.mIndices[2] + VertexOffset;
                  });

    aiMaterial* assimpMaterial = AiScene->mMaterials[AiMesh->mMaterialIndex];
    String materialName = assimpMaterial->GetName().C_Str();

    MeshImportData::Mesh::SubMesh subMesh;
    subMesh.startIndex = IndexOffset;
    subMesh.indexCount = static_cast<uint32>(Indices.size() - IndexOffset);
    subMesh.materialName = materialName;

    IndexOffset = static_cast<uint32>(Indices.size());
    VertexOffset += AiMesh->mNumVertices;

    return subMesh;
}
