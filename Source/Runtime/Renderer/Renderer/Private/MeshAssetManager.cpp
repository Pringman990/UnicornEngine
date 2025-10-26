#include "MeshAssetManager.h"

#include <Assets/AssetRegistry.h>
#include "MeshDecoder.h"
#include "Renderer.h"
#include "MeshManager.h"

REGISTER_ASSET_MANAGER(MeshAssetManager, (MeshAsset), ("fbx"));

MeshAssetManager::MeshAssetManager()
{
}

MeshAssetManager::~MeshAssetManager()
{
}

GenericHandle MeshAssetManager::Load(const String& VirtualPath)
{

    auto it = mPathToHandle.find(VirtualPath);
    if (it != mPathToHandle.end())
    {
        return ToGenericHandle(it->second);
    }

    AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

    if (!readData.UUID.IsValid())
    {
        LOG_ERROR("Trying to load asset without uuid");
        return GenericHandle::Invalid();
    }

    AssetHandle<MeshAsset> handle = mMeshPool.Allocate(readData.UUID);
    WeakPtr<MeshAsset> ptr = mMeshPool.Get(handle);
    if (auto mesh = ptr.lock())
    {
        mesh->SetMetaPath(VirtualPath);
        mesh->SetSourcePath(readData.SourcePath);

        ByteBuffer meshData = GET_FILESYSTEM()->ReadAll(readData.SourcePath);
        MeshDecodeData decodeData = MeshDecoder::LoadMesh(meshData, "fbx");
        if (!decodeData.IsValid())
        {
            LOG_ERROR("Loading mesh asset failed trying to import source");
            mMeshPool.Remove(handle);
            return GenericHandle::Invalid();
        }

        //TODO: add suppport to import a mesh on a specific index and not just 0
        GPUResourceHandle<GPUMesh> gpuMesh = SubsystemManager::Get<Renderer>()->GetMeshManager()->CreateMesh(decodeData.meshes[0]);
        if (!gpuMesh)
        {
            LOG_ERROR("Loading mesh asset failed trying to import source");
            mMeshPool.Remove(handle);
            return GenericHandle::Invalid();
        }
        mesh->SetGPUMeshHandle(gpuMesh);
    }
    else
    {
        LOG_WARNING("Asset was invalid when just created, path: {}", VirtualPath);
    }

    mPathToHandle[VirtualPath] = handle;

    return ToGenericHandle(handle);
}

GenericHandle MeshAssetManager::ImportSource(const String& VirtualSourcePath)
{
    ByteBuffer meshData = GET_FILESYSTEM()->ReadAll(VirtualSourcePath);
    MeshDecodeData decodeData = MeshDecoder::LoadMesh(meshData, "fbx");
    if (!decodeData.IsValid())
    {
        return GenericHandle::Invalid();
    }

    //TODO: add suppport to import all meshes from a file
    GPUResourceHandle<GPUMesh> mesh = SubsystemManager::Get<Renderer>()->GetMeshManager()->CreateMesh(decodeData.meshes[0]);

    UniqueID128 uuid = UniqueID128::FromRandom();
    AssetHandle<MeshAsset> handle = mMeshPool.Allocate(uuid);
    WeakPtr<MeshAsset> ptr = mMeshPool.Get(handle);
    if (auto asset = ptr.lock())
    {
        asset->SetSourcePath(VirtualSourcePath);
        asset->SetGPUMeshHandle(mesh);
    }

    String assetPath = ExtractPathWithoutExtension(VirtualSourcePath);
    assetPath += ".asset";
    AssetRegistry::CreateAssetFile(assetPath, "mesh", uuid, {}, VirtualSourcePath);

    return ToGenericHandle(handle);
}

GenericHandle MeshAssetManager::ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID)
{
    THROW("Not Implemeted");
    return GenericHandle();
}

bool MeshAssetManager::UnLoad(const String& VirtualPath)
{
    THROW("Not Implemeted");
    return false;
}

UniqueID128 MeshAssetManager::FindAssetBySource(const String& VirtualSourcePath)
{
    auto it = mPathToHandle.find(VirtualSourcePath);
    if (it == mPathToHandle.end())
    {
        return UniqueID128();
    }

    WeakPtr<MeshAsset> ptr = mMeshPool.Get(it->second);
    if (auto asset = ptr.lock())
    {
        return asset->GetUUID();
    }

    return UniqueID128();
}

bool MeshAssetManager::Exists(const String& VirtualPath) const
{
    return mPathToHandle.contains(VirtualPath);
}

WeakPtr<MeshAsset> MeshAssetManager::GetAssetPtr(AssetHandle<MeshAsset> Handle)
{
    return mMeshPool.Get(Handle);
}
