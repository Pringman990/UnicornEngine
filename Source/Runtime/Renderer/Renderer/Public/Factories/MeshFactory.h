#pragma once
#include <Core.h>

#include "GPUMesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace Assimp
{
	class Importer;
}

class MeshFactory
{
public:
	/// <summary>
	/// Normaly used for loading ex fbx files as it can contain multiple meshes
	/// </summary>
	/// <param name="MeshData"></param>
	/// <returns></returns>
	static const Vector<GPUResourceHandle<GPUMesh>> CreateMeshes(ByteBuffer& MeshData);

	/// <summary>
	/// Used to ensure loading of only a single mesh that matches the name
	/// </summary>
	/// <param name="MeshData"></param>
	/// <param name="MeshName"></param>
	/// <returns></returns>
	static const GPUResourceHandle<GPUMesh> CreateMesh(ByteBuffer& MeshData, const String& MeshName);

private:
	static const aiScene* GetScene(Assimp::Importer& Importer, ByteBuffer& MeshData);
	static aiNode* GetNodeByName(aiNode* Node, const String& NodeName);
	static void ProcessAiNodes(aiNode* Node, const aiScene* Scene, Vector<GPUResourceHandle<GPUMesh>>& Meshes, const String& MeshName);
	static void ConvertAiMeshToMesh(
		aiMesh* AiMesh,
		const aiScene* Scene,
		uint32& IndexOffset,
		Vector<Vertex3D>& Vertices,
		Vector<uint32>& Indices,
		Vector<GPUMesh::GPUSubMesh>& SubMeshes
	);

};