#include "MeshManager.h"

#include "Renderer.h"
#include "RenderBufferManager.h"

MeshManager::MeshManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

MeshManager::~MeshManager()
{
}

GPUResourceHandle<GPUMesh> MeshManager::CreateFromRaw(const Vector<Vertex>& Verticies, const Vector<uint32>& Indicies, AssetHandle<Material> Material)
{
	GPUResourceHandle<GPUMesh> handle = mMeshes.Allocate();
	GPUMesh* mesh = mMeshes.Get(handle);
	mesh->vertexBuffer = mRenderer->GetBufferManager()->CreateVertex(Verticies);
	mesh->indexBuffer = mRenderer->GetBufferManager()->CreateIndex(Indicies);
	
	GPUMesh::Submesh submesh;
	submesh.startIndex = 0;
	submesh.indexCount = static_cast<uint32>(Indicies.size());
	submesh.materialHandle = Material;
	mesh->submeshes.push_back(submesh);

	return handle;
}
