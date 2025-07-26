#pragma once
#include <Core.h>
#include <AssetManager/AssetLoader.h>

#include "Mesh.h"
#include "Vertex.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class MeshLoader : public AssetLoader<Mesh>
{
public:
	MeshLoader();
	~MeshLoader() override;

	AssetLoadResult<Mesh> Load(const String& VirtualPath, Mesh* Asset) override;
	bool Save(const String& VirtualPath, AssetHandle<Mesh> Handle) override;

private:
	void ProcessAiNodes(aiNode* Node, const aiScene* Scene, AssetLoadResult<Mesh>& Result);
	void ConvertAiMeshToMesh(
		aiMesh* AiMesh,
		const aiScene* Scene,
		AssetLoadResult<Mesh>& Result,
		uint32& IndexOffset,
		Vector<Vertex3D>& Vertices,
		Vector<uint32>& Indices
	);
};