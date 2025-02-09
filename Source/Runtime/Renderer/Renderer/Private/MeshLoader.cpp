#include "pch.h"
#include "MeshLoader.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Renderer.h"
#include <execution>

MeshLoader::MeshLoader()
{
}

MeshLoader::~MeshLoader()
{
}

bool MeshLoader::LoadScene(const std::string& aFilePath, std::vector<std::shared_ptr<Mesh>>& someMesh)
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
	ProcessAiNodes(scene->mRootNode, scene, someMesh);

	return true;
}

void MeshLoader::ProcessAiNodes(aiNode* aNode, const aiScene* aScene, std::vector<std::shared_ptr<Mesh>>& someMesh)
{
	if (aNode->mNumMeshes > 0)
	{
		std::shared_ptr<Mesh> mesh =  std::make_shared<Mesh>();
		uint32 indexOffset = 0;
		uint32 vertexOffset = 0;
		std::vector<Vertex> vertices;
		std::vector<uint32> indices;
		
		//This represent ONE Mesh
		for (uint32 i = 0; i < aNode->mNumMeshes; i++)
		{
			aiMesh* aiMesh = aScene->mMeshes[aNode->mMeshes[i]];
			ConvertAiMeshToMesh(aiMesh, aScene, mesh, indexOffset, vertexOffset, vertices, indices);
		}
		
		//mesh->mVertexCount = static_cast<uint32>(vertices.size());
		mesh->mIndexBuffer = Renderer::GetInstance()->CreateIndexBuffer(indices);
		mesh->mVertexBuffer = Renderer::GetInstance()->CreateVertexBuffer(vertices);

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
	const aiScene* /*aScene*/,
	std::shared_ptr<Mesh> aMesh,
	uint32& aIndexOffset,
	uint32& aVertexOffset,
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
	size_t initalSize = someIndices.size();
	someIndices.resize((initalSize + (aAiMesh->mNumFaces * 3)));
	
	std::for_each(std::execution::par, aAiMesh->mFaces, aAiMesh->mFaces + aAiMesh->mNumFaces, 
		[&](const aiFace& face) { 
			size_t faceIndex = &face - aAiMesh->mFaces; 
			size_t globalOffset = initalSize + (faceIndex * 3);
	
			someIndices[globalOffset] = face.mIndices[0] + aVertexOffset;
			someIndices[globalOffset + 1] = face.mIndices[1] + aVertexOffset;
			someIndices[globalOffset + 2] = face.mIndices[2] + aVertexOffset;
		});

	std::shared_ptr<Material> material;
	if (aAiMesh->mMaterialIndex >= 0)
	{
		//aiMaterial* assimpMaterial = aScene->mMaterials[aAiMesh->mMaterialIndex];
		material = Material::CreateDefaultPolygon();
	}

	SubMesh subMesh;
	subMesh.startIndex = aIndexOffset;
	subMesh.indexCount = static_cast<uint32>(someIndices.size() - aIndexOffset);
	subMesh.material = material;
	 
	aMesh->mSubMeshes.push_back(subMesh);

	aIndexOffset = static_cast<uint32>(someIndices.size());
	aVertexOffset += aAiMesh->mNumVertices;
}
