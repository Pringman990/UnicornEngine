#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Vertex.h"

struct aiNode;
struct aiMesh;
struct aiScene;

struct MeshDecodeData
{
	struct Mesh
	{
		struct SubMesh
		{
			uint32 startIndex;
			uint32 indexCount;
		};

		Vector<SubMesh> submeshes;
		Vector<Vertex> vertices;
		Vector<uint32> indices;
	};

	Vector<Mesh> meshes;

	bool IsValid() const
	{
		return meshes.size() > 0;
	}
};

/**
* Decoder for multiple different mesh formats.
*
* @info
* Supported image formats include: fbx.
*/
class MeshDecoder
{
public:

	/**
	* Loads mesh data from buffer.
	* MeshDecodeData will be invalid if the buffer could not be read.
	*/
	static MeshDecodeData LoadMesh(const ByteBuffer& Buffer, const String& Extension);

	static inline constexpr bool IsFormatSupported(const String& Format) { return IsAnyOf(Format, "fbx"); }

private:
	static MeshDecodeData AssimpDecoder(const ByteBuffer& Buffer, const String& Extension);
	static void ProcessAssimpNode(MeshDecodeData& DecodeData, aiNode* AiNode, const aiScene* AiScene);
	static MeshDecodeData::Mesh::SubMesh DecodeAiMesh(aiMesh* AiMesh, uint32& IndexOffset, uint32& VertexOffset, Vector<uint32>& Indices, Vector<Vertex>& Vertices);

};