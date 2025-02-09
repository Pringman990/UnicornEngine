#pragma once
#include "Mesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class MeshLoader
{
public:
	static bool LoadScene(const std::string& aFilePath, std::vector<std::shared_ptr<Mesh>>& someMesh);

private:
	MeshLoader();
	~MeshLoader();

	static void ProcessAiNodes(aiNode* aNode, const aiScene* aScene, std::vector<std::shared_ptr<Mesh>>& someMesh);
	static void ConvertAiMeshToMesh(
		aiMesh* aAiMesh,
		const aiScene* aScene, 
		std::shared_ptr<Mesh> aMesh, 
		uint32& aIndexOffset, 
		uint32& aVertexOffset,
		std::vector<Vertex>& aVertices, 
		std::vector<uint32>& aIndices
	);
};