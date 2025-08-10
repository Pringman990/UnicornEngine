#pragma once
#include <Core.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh
{
public:
	struct SubMesh
	{
		uint32 indexOffest;
		uint32 indexCount;
	};
public:
	Mesh();
	~Mesh();

private:
	VertexBuffer mVertexBuffer;
	IndexBuffer mIndexBuffer;
	Vector<SubMesh> mSubMeshes;
};