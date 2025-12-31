#pragma once
#include <EngineMinimal.h>
#include <Assets/AssetBase.h>
#include <Assets/AssetRef.h>
#include "RendererMinimal.h"

#include "GPUResources/GPUMesh.h"
#include "Vertex.h"

class Material;

class Mesh : public AssetBase
{
public:
	struct Submesh
	{
		uint32 startIndex = 0;
		uint32 indexCount = 0;
		AssetRef<Material> material;
	};

public:
	Mesh();
	Mesh(UniqueID128 UUID);
	~Mesh() override;

	void SetMaterial(uint32 SubmeshIndex, AssetRef<Material> Material);
	AssetRef<Material> GetMaterial(uint32 SubmeshIndex) const;

	const Vector<Submesh>& GetSubmeshes() const { return mSubmeshes; }
	void FillSubmeshes(const Vector<Mesh::Submesh>& Submeshes);

	void SetGPUMeshHandle(GPUResourceHandle<GPUMesh> Handle) { mGPUMesh = Handle; };
	GPUResourceHandle<GPUMesh> GetGPUMeshHandle() const { return mGPUMesh; };

private:
	GPUResourceHandle<GPUMesh> mGPUMesh;

	Vector<Submesh> mSubmeshes;
	Vector<Vertex> mVerticies;
	Vector<uint32> mIndicies;
};

REFL_DECLARE(Mesh)
REFL_DECLARE(AssetRef<Mesh>)
