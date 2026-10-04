#pragma once
#include "assimp/scene.h"
#include "Renderer/Renderer.h"

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
    };

    List<Mesh> meshes;
};

class MeshImporter
{
public:
    static Expected<MeshImportData, String> Import(const ByteBuffer& data, const String& hint);

private:
    static void ImportMeshes(
        MeshImportData& importData,
        const aiScene* scene,
        const aiNode* node
        );

    static MeshImportData::Mesh::SubMesh ImportAiMesh(
        MeshImportData& ImportData,
        const aiScene* AiScene,
        aiMesh* AiMesh, uint32& IndexOffset,
        uint32& VertexOffset, List<uint32>& Indices,
        List<Vertex>& Vertices,
        MeshImportData::Mesh& mesh
    );
};
