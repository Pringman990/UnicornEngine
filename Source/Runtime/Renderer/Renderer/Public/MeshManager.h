#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUMesh.h"
#include "Vertex.h"
#include "MeshDecoder.h"

class Renderer;

/**
* Manager for GPUMesh and not Mesh assets.
*/
class MeshManager
{
public:
	MeshManager(Renderer* InRenderer);
	~MeshManager();

	GPUResourceHandle<GPUMesh> LoadMesh(const String& VirtualPath);

	GPUResourceHandle<GPUMesh> CreateFromRaw(const Vector<Vertex>& Verticies, const Vector<uint32>& Indicies, AssetHandle<Material> Material);
	
	GPUResourceHandle<GPUMesh> CreateMesh(const MeshDecodeData::Mesh& DecodedMesh);


	inline GPUMesh* GetInternalMesh(GPUResourceHandle<GPUMesh> Handle)
	{
		return mMeshes.Get(Handle);
	}

private:
	Renderer* mRenderer;
	ResourcePool<GPUMesh> mMeshes;
};