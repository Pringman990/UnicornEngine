#pragma once
#include <IAssetManager.h>

#include "AssetRegistry.h"
#include "Mesh.h"

class MeshResourceManager : public IAssetManager<Mesh>
{
	friend class AssetRegistry;
public:
	virtual Mesh* LoadAsset() override;

	std::vector<Mesh*> LoadMeshesFromFile(const std::string& aFilePath);
	Mesh* GetMesh(const std::string& aFileAssetPath);

private:
	MeshResourceManager();
	~MeshResourceManager() override;
private:
	std::unordered_map<std::string, Mesh*> mAssetMeshes;
	std::unordered_map<std::string, std::vector<Mesh*>> mMeshesFromFile;
};