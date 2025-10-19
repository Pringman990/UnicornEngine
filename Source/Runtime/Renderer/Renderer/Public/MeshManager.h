#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUMesh.h"
#include "Vertex.h"

class Renderer;

class MeshManager
{
public:
	MeshManager(Renderer* InRenderer);
	~MeshManager();

	GPUResourceHandle<GPUMesh> LoadMesh(const String& VirtualPath);

	GPUResourceHandle<GPUMesh> CreateFromRaw(const Vector<Vertex>& Verticies, const Vector<uint32>& Indicies, AssetHandle<Material> Material);

	inline GPUMesh* GetInternalMesh(GPUResourceHandle<GPUMesh> Handle)
	{
		return mMeshes.Get(Handle);
	}

private:
	Renderer* mRenderer;
	ResourcePool<GPUMesh> mMeshes;
};