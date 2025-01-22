#pragma once
#include "Material.h"
#include "Buffers/RenderBuffer.h"

#include <StandardTypes/StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>

struct SubMesh
{
	Material* material = nullptr;
	uint32 startIndex = 0;
	uint32 indexCount = 0;
};

class Mesh
{
	friend class MeshLoader;
	friend class MeshResourceManager;

public:


	void SetMaterial(uint32 aIndex, Material* aMaterial);
	Material* GetMaterial(uint32 aIndex);

	const std::vector<SubMesh>& GetSubMeshes() const;

	RenderBuffer* GetVertexBuffer();
	RenderBuffer* GetIndexBuffer();
private:
	Mesh();
	~Mesh();

protected:
	std::vector<SubMesh> mSubMeshes;
	
	/**
	* We use vertex count for displaying in editor.
	* Not used for rendering as we use DrawIndexed and not Draw.
	*/
	uint32_t mVertexCount;

	RenderBuffer* mVertexBuffer;
	RenderBuffer* mIndexBuffer;
};