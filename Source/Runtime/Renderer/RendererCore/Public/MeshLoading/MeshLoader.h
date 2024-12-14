#pragma once
#include "Mesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class MeshLoader
{
public:
	static bool LoadScene(const std::string& aFilePath, std::vector<Mesh*>& someMesh);

private:
	MeshLoader();
	~MeshLoader();

	static void ProcessAiNodes(aiNode* aNode, const aiScene* aScene, std::vector<Mesh*>& someMesh);
	static void ConvertAiMeshToMesh(
		aiMesh* aAiMesh,
		const aiScene* aScene, 
		Mesh* aMesh, 
		uint32& aIndexOffset, 
		std::vector<Vertex>& aVertices, 
		std::vector<uint32>& aIndices
	);

	static Material* LoadMaterial(aiMaterial* aAiMaterial);
};