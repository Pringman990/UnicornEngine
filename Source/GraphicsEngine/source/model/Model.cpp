#include "GraphicsPch.h"
#include "Model.h"

#include "source/Material.h"

Model::Model()
{
}

Model::~Model()
{
}

void Model::Init(std::vector<MeshData>& someMeshData)
{
	mMeshData = someMeshData;
}

bool Model::SetMaterial(int aMeshIndex, const std::string& aColorPath, const std::string& aMaterialPath, const std::string& aNormalPath)
{
	return mMeshData[aMeshIndex].material->CreateMaterial(aColorPath, aMaterialPath, aNormalPath);
}

void Model::SetMeshData(int aMeshIndex, MeshData someData)
{
	mMeshData[aMeshIndex] = someData;
}

void Model::BindMaterials()
{
	for (int i = 0; i < mMeshData.size(); i++)
	{
		mMeshData[i].material->Bind();
	}
}