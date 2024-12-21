#include "pch.h"
#include "MeshLoading/MeshLoader.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Renderer.h"
#include "MaterialResourceManager.h"

#include <execution>

MeshLoader::MeshLoader()
{
}

MeshLoader::~MeshLoader()
{
}

bool MeshLoader::LoadScene(const std::string& aFilePath, std::vector<Mesh*>& someMesh)
{
	Assimp::Importer importer;

	const uint32 readFlags =
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	const aiScene* scene = importer.ReadFile(aFilePath.c_str(), readFlags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::string error = importer.GetErrorString();
		_LOG_RENDERER_ERROR("Cant load file, Error: {}", error);
		return false;
	}

	someMesh.reserve(scene->mNumMeshes);

	//const aiMetadata* metadata = scene->mMetaData;
	//if (metadata) {
	//	for (unsigned int i = 0; i < metadata->mNumProperties; ++i) {
	//		std::string key(metadata->mKeys[i].data);
	//		if (key == "UnitScaleFactor") { // Example for FBX files
	//			float scaleFactor = *static_cast<float*>(metadata->mValues[i].mData);
	//			std::cout << "Unit Scale Factor: " << scaleFactor << std::endl;
	//		}
	//	}
	//}

	ProcessAiNodes(scene->mRootNode, scene, someMesh);

	return true;
}

void MeshLoader::ProcessAiNodes(aiNode* aNode, const aiScene* aScene, std::vector<Mesh*>& someMesh)
{
	if (aNode->mNumMeshes > 0)
	{
		Mesh* mesh = new Mesh();
		uint32 indexOffset = 0;
		std::vector<Vertex> vertices;
		std::vector<uint32> indices;
		
		//This represent ONE Mesh
		for (uint32 i = 0; i < aNode->mNumMeshes; i++)
		{
			aiMesh* aiMesh = aScene->mMeshes[aNode->mMeshes[i]];
			ConvertAiMeshToMesh(aiMesh, aScene, mesh, indexOffset, vertices, indices);
		}
		
		mesh->mVertexCount = static_cast<uint32>(vertices.size());
		mesh->mIndexBuffer = Renderer::GetInstance()->GetRenderBufferFactory()->CreateIndexBuffer(indices);
		mesh->mVertexBuffer = Renderer::GetInstance()->GetRenderBufferFactory()->CreateVertexBuffer(vertices);

		someMesh.emplace_back(mesh);
	}

	//These are new meshes for rendering but in the hierarchy they are children
	for (uint32 i = 0; i < aNode->mNumChildren; i++)
	{
		ProcessAiNodes(aNode->mChildren[i], aScene, someMesh);
	}
}

void MeshLoader::ConvertAiMeshToMesh(
	aiMesh* aAiMesh,
	const aiScene* aScene,
	Mesh* aMesh,
	uint32& aIndexOffset,
	std::vector<Vertex>& someVertices,
	std::vector<uint32>& someIndices
)
{
	for (uint32 i = 0; i < aAiMesh->mNumVertices; i++)
	{
		Vertex vertex;
		aiVector3D aiVertex = aAiMesh->mVertices[i];

		vertex.position = Vector4(aiVertex.x, aiVertex.y, aiVertex.z, 1);
		
		if (aAiMesh->HasNormals())
		{
			aiVector3D aiNormals = aAiMesh->mNormals[i];
			vertex.normal = Vector3(aiNormals.x, aiNormals.y, aiNormals.z);
		}

		if (aAiMesh->mTextureCoords[0])
		{
			aiVector3D aiUV = aAiMesh->mTextureCoords[0][i];
			vertex.uv = Vector2(aiUV.x, aiUV.y);
		}

		if (aAiMesh->HasTangentsAndBitangents())
		{
			aiVector3D aiTangent = aAiMesh->mTangents[i];
			aiVector3D aiBiTangent = aAiMesh->mBitangents[i];
			vertex.tangent = Vector3(aiTangent.x, aiTangent.y, aiTangent.z);
			vertex.bitangent = Vector3(aiBiTangent.x, aiBiTangent.y, aiBiTangent.z);
		}

		someVertices.push_back(vertex);
	}

	//We know it will always be a max of 3 indcies per face as we us aiProcess_Triangulate when importing the scene
	//someIndices.resize(aIndexOffset + (aAiMesh->mNumFaces * 3));

	//std::for_each(std::execution::par, aAiMesh->mFaces, aAiMesh->mFaces + aAiMesh->mNumFaces,
	//	[&](const aiFace& face) {
	//		size_t faceIndex = &face - aAiMesh->mFaces;
	//		size_t globalOffset = aIndexOffset + (faceIndex * 3);

	//		someIndices[globalOffset] = face.mIndices[0];
	//		someIndices[globalOffset + 1] = face.mIndices[1];
	//		someIndices[globalOffset + 2] = face.mIndices[2];
	//	});

	for (UINT i = 0; i < aAiMesh->mNumFaces; i++) {
		aiFace face = aAiMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			someIndices.push_back(face.mIndices[j] + aIndexOffset);
	}

	Material* material = nullptr;
	if (aAiMesh->mMaterialIndex >= 0)
	{
		aiMaterial* assimpMaterial = aScene->mMaterials[aAiMesh->mMaterialIndex];
		material = LoadMaterial(assimpMaterial);
	}

	SubMesh subMesh;
	subMesh.startIndex = aIndexOffset;
	subMesh.indexCount = static_cast<uint32>(someIndices.size() - aIndexOffset);
	subMesh.material = material;
	 
	aMesh->mSubMeshes.push_back(subMesh);

	aIndexOffset += static_cast<uint32>(aAiMesh->mNumVertices);
}

Material* MeshLoader::LoadMaterial(aiMaterial* /*aAiMaterial*/)
{
	//Material* material = new Material();

	//aiString texturePath;

	//// Load diffuse texture
	//if (assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
	//	material->diffuseTexture = LoadTexture(texturePath.C_Str());
	//}

	//// Load other textures (e.g., specular, normal, etc.)
	//if (assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath) == AI_SUCCESS) {
	//	material->specularTexture = LoadTexture(texturePath.C_Str());
	//}
	MaterialResourceManager* materialManager = GET_RESOURCE_MANAGER(MaterialResourceManager);
	Material* material = materialManager->GetMaterial("__UNCE_Defualt_Material");
	return material;
}
