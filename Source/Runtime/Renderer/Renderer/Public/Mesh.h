#pragma once
#include <Core.h>
#include <AssetManager/AssetBase.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh : public AssetBase<Mesh>
{
	friend class AssetManager;
public:
	struct SubMesh
	{
		uint32 indexOffset = 0;
		uint32 vertexOffset = 0;
		uint32 indexCount = 0;
		String name;
	};

	struct Metadata
	{
		String name = "";

		uint32 vertexCount = 0;
		uint32 indexCount = 0;
	};

	struct ResourceData
	{
		VertexBuffer mVertexBuffer;
		IndexBuffer mIndexBuffer;

		Vector<SubMesh> mSubMeshes;
	};

	struct StagingData
	{
		Vector<Vertex3D> vertices;
		Vector<uint32> indicies;
	};

public:
	Mesh() {};
	~Mesh() override {};
	
	const String& GetName() const override { return mMetadata.name; }

	const Mesh::Metadata& GetMetadata() const { return mMetadata; };

	const Vector<SubMesh>& GetSubMeshes() const { return mResourceData.mSubMeshes; };

	void FreeStagingData();

private:
	Metadata mMetadata;
	ResourceData mResourceData;
	StagingData mStagingData;
};