#pragma once
#include <EngineMinimal.h>
#include <Assets/AssetBase.h>
#include "RendererMinimal.h"

#include "GPUMesh.h"

class MeshAsset : public AssetBase
{
public:
	MeshAsset();
	MeshAsset(UniqueID128 UUID);
	~MeshAsset();

	void SetGPUMeshHandle(GPUResourceHandle<GPUMesh> Handle) { mGPUMesh = Handle; };
	GPUResourceHandle<GPUMesh> GetGPUMeshHandle() const { return mGPUMesh; };

private:
	GPUResourceHandle<GPUMesh> mGPUMesh;
};