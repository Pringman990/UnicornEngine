#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Assets/IAssetManager.h"
#include "MeshAsset.h"

class MeshAssetManager : public IAssetManager
{
public:
	MeshAssetManager();
	~MeshAssetManager() override;

	virtual GenericHandle Load(const String& VirtualPath) override;
	virtual GenericHandle ImportSource(const String& VirtualSourcePath) override;
	virtual GenericHandle ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID) override;
	virtual bool UnLoad(const String& VirtualPath) override;
	virtual UniqueID128 FindAssetBySource(const String& VirtualSourcePath) override;
	virtual bool Exists(const String& VirtualPath) const override;

	WeakPtr<MeshAsset> GetAssetPtr(AssetHandle<MeshAsset> Handle);

private:
	HeapResourcePool<MeshAsset> mMeshPool;
	UnorderedMap<String, AssetHandle<MeshAsset>> mPathToHandle;
};