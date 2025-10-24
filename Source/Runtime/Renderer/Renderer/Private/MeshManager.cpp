#include "MeshManager.h"

#include "Renderer.h"
#include "RenderBufferManager.h"

//TODO: Remove when materials are loaded with mesh
#include "MaterialManager.h"

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

GPUResourceHandle<GPUMesh> MeshManager::CreateMesh(const MeshDecodeData::Mesh& DecodedMesh)
{
	GPUResourceHandle<GPUMesh> handle = mMeshes.Allocate();
	GPUMesh* mesh = mMeshes.Get(handle);
	mesh->vertexBuffer = mRenderer->GetBufferManager()->CreateVertex(DecodedMesh.vertices);
	mesh->indexBuffer = mRenderer->GetBufferManager()->CreateIndex(DecodedMesh.indices);

	mesh->submeshes.resize(DecodedMesh.submeshes.size());
	for (int32 i = 0; i < DecodedMesh.submeshes.size(); i++)
	{
		GPUMesh::Submesh submesh;
		submesh.startIndex = DecodedMesh.submeshes[i].startIndex;
		submesh.indexCount = static_cast<uint32>(DecodedMesh.submeshes[i].indexCount);
		submesh.materialHandle = mRenderer->GetMaterialManager()->GetEngineMaterial("textured_mesh");
		mesh->submeshes[i] = submesh;
	}

	return handle;
}
