#include "RenderAssets/MeshManager.h"

#include <Assets/AssetRegistry.h>
#include "MeshDecoder.h"
#include "Renderer.h"
#include "GPUResources/GPUMeshManager.h"
#include "GPUResources/GPUMaterialManager.h"

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

Mesh* MeshManager::Load(const String& VirtualPath)
{
	AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

	if (!readData.UUID.IsValid())
	{
		LOG_ERROR("Trying to load asset without uuid");
		return nullptr;
	}

	Mesh* mesh = new Mesh(readData.UUID);
	mesh->SetMetaPath(VirtualPath);
	mesh->SetSourcePath(readData.SourcePath);
	mesh->SetName(readData.Name);
	mesh->SetType(readData.Type);

	ByteBuffer meshData = GET_FILESYSTEM()->ReadAll(readData.SourcePath);
	MeshDecodeData decodeData = MeshDecoder::LoadMesh(meshData, "fbx");
	if (!decodeData.IsValid())
	{
		LOG_ERROR("Loading mesh asset failed trying to import source");
		return nullptr;
	}

	//TODO: add suppport to import a mesh on a specific index and not just 0
	GPUResourceHandle<GPUMesh> gpuMesh = SubsystemManager::Get<Renderer>()->GetGPUMeshManager()->CreateMesh(decodeData.meshes[0]);
	if (!gpuMesh)
	{
		LOG_ERROR("Loading mesh asset failed trying to import source");
		delete mesh;
		return nullptr;
	}
	mesh->SetGPUMeshHandle(gpuMesh);

	Vector<Mesh::Submesh> submeshes;
	submeshes.resize(decodeData.meshes[0].submeshes.size());

	readData.CustomData->BeginReadArray("materials");
	for (int32 i = 0; i < decodeData.meshes[0].submeshes.size(); i++)
	{
		Mesh::Submesh submesh;
		submesh.startIndex = decodeData.meshes[0].submeshes[i].startIndex;
		submesh.indexCount = static_cast<uint32>(decodeData.meshes[0].submeshes[i].indexCount);

		readData.CustomData->BeginReadObject();
		
		String uuidStr;
		readData.CustomData->ReadString(uuidStr, "uuid");
		UniqueID128 uuid = UniqueID128(uuidStr);
		if (uuid.IsValid())
		{
			submesh.material = GET_ASSETREGISTRY()->Load<Material>(uuid);
		}

		submeshes[i] = submesh;
	}
	readData.CustomData->EndReadArray();
	mesh->FillSubmeshes(submeshes);

	return mesh;
}

Mesh* MeshManager::ImportSource(const String& VirtualSourcePath)
{
	ByteBuffer meshData = GET_FILESYSTEM()->ReadAll(VirtualSourcePath);
	MeshDecodeData decodeData = MeshDecoder::LoadMesh(meshData, "fbx");
	if (!decodeData.IsValid())
	{
		return nullptr;
	}

	//TODO: add suppport to import all meshes from a file
	GPUResourceHandle<GPUMesh> mesh = SubsystemManager::Get<Renderer>()->GetGPUMeshManager()->CreateMesh(decodeData.meshes[0]);

	UniqueID128 uuid = UniqueID128::FromRandom();
	Mesh* asset = new Mesh(uuid);
	asset->SetSourcePath(VirtualSourcePath);
	asset->SetGPUMeshHandle(mesh);
	
	String type = ExtractTypeInfoNameWithoutSpecifier(typeid(Mesh).name());
	asset->SetType(type);

	String assetName = ExtractPathWithoutExtension(VirtualSourcePath);
	String assetPath = ExtractPathWithoutExtension(VirtualSourcePath);
	assetPath += ".asset";
	AssetRegistry::CreateAssetFile(assetPath, type, assetName, uuid, {}, VirtualSourcePath);

	return asset;
}

Mesh* MeshManager::ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID)
{
	THROW("Not Implemeted");
	return nullptr;
}

bool MeshManager::UnLoad(const String& VirtualPath)
{
	THROW("Not Implemeted");
	return false;
}

