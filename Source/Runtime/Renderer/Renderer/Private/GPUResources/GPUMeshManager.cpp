#include "GPUResources/GPUMeshManager.h"

#include "Renderer.h"
#include "RenderBufferManager.h"

//TODO: Remove when materials are loaded with mesh
#include "GPUResources/GPUMaterialManager.h"
#include <Assets/AssetRegistry.h>
#include <RenderAssets/Material.h>

GPUMeshManager::GPUMeshManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

GPUMeshManager::~GPUMeshManager()
{
}

GPUResourceHandle<GPUMesh> GPUMeshManager::CreateFromRaw(const Vector<Vertex>& Verticies, const Vector<uint32>& Indicies)
{
	GPUResourceHandle<GPUMesh> handle = mMeshes.Allocate();
	GPUMesh* mesh = mMeshes.Get(handle);
	mesh->vertexBuffer = mRenderer->GetBufferManager()->CreateVertex(Verticies);
	mesh->indexBuffer = mRenderer->GetBufferManager()->CreateIndex(Indicies);

	return handle;
}

GPUResourceHandle<GPUMesh> GPUMeshManager::CreateMesh(const MeshDecodeData::Mesh& DecodedMesh)
{
	GPUResourceHandle<GPUMesh> handle = mMeshes.Allocate();
	GPUMesh* mesh = mMeshes.Get(handle);
	mesh->vertexBuffer = mRenderer->GetBufferManager()->CreateVertex(DecodedMesh.vertices);
	mesh->indexBuffer = mRenderer->GetBufferManager()->CreateIndex(DecodedMesh.indices);

	return handle;
}
