#pragma once
#include <IResourceManager.h>

#include "ResourceRegistry.h"
#include "Mesh.h"

class MeshResourceManager : public IResourceManager
{
public:
	MeshResourceManager();
	~MeshResourceManager() override;

	std::vector<Mesh*> LoadMeshesFromFile(const std::string& aFilePath);
	Mesh* GetMesh(const std::string& aFileAssetPath);

private:
	std::unordered_map<std::string, Mesh*> mAssetMeshes;
	std::unordered_map<std::string, std::vector<Mesh*>> mMeshesFromFile;
};

//REGISTER_RESOURCEMANAGER(MeshResourceManager)