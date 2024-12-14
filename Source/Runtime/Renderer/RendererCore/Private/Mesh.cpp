#include "pch.h"
#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	mVertexBuffer->Release();
	mIndexBuffer->Release();
}

void Mesh::SetMaterial(uint32 aIndex, Material* aMaterial)
{
	if(aIndex > mSubMeshes.size())
	{
		_LOG_RENDERER_WARNING("Material index out of range");
		return;
	}

	mSubMeshes[aIndex].material = aMaterial;
}

Material* Mesh::GetMaterial(uint32 aIndex)
{
	if (aIndex > mSubMeshes.size())
	{
		_LOG_RENDERER_WARNING("Material index out of range");
		return nullptr;
	}

	return mSubMeshes[aIndex].material;
}

const std::vector<SubMesh>& Mesh::GetSubMeshes() const
{
	return mSubMeshes;
}

RenderBuffer* Mesh::GetVertexBuffer() 
{
	return mVertexBuffer;
}

RenderBuffer* Mesh::GetIndexBuffer() 
{
	return mIndexBuffer;
}
