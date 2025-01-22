#include "pch.h"
#include "MeshLoading/MeshResourceManager.h"

#include "MeshLoading/MeshLoader.h"

MeshResourceManager::MeshResourceManager()
{
}

MeshResourceManager::~MeshResourceManager()
{
	for (auto& it : mMeshesFromFile)
	{
		for (uint32 i = 0; i < it.second.size(); i++)
		{
			delete it.second[i];
			it.second[i] = nullptr;
		}
		it.second.clear();
	}
	mMeshesFromFile.clear();
}

Mesh* MeshResourceManager::LoadAsset()
{
	return nullptr;
}

std::vector<Mesh*> MeshResourceManager::LoadMeshesFromFile(const std::string& aFilePath)
{

	std::vector<Mesh*> meshes;
	MeshLoader::LoadScene(aFilePath, meshes);
	mMeshesFromFile[aFilePath] = meshes;

	return mMeshesFromFile[aFilePath];
}

Mesh* MeshResourceManager::GetMesh(const std::string& aFileAssetPath)
{
	auto it = mAssetMeshes.find(aFileAssetPath);
	if (it != mAssetMeshes.end())
	{
		return it->second;
	}

	return nullptr;
}
