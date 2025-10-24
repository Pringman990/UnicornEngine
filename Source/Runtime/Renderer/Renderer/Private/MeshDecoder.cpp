#include "MeshDecoder.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <execution>

MeshDecodeData MeshDecoder::LoadMesh(const ByteBuffer& Buffer, const String& Extension)
{
	if (Extension == "fbx")
	{
		return AssimpDecoder(Buffer, Extension);
	}

	return MeshDecodeData();
}

MeshDecodeData MeshDecoder::AssimpDecoder(const ByteBuffer& Buffer, const String& Extension)
{
	Assimp::Importer importer;

	const uint32 readFlags =
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	const aiScene* scene = importer.ReadFileFromMemory(Buffer.data(), Buffer.size(), readFlags, Extension.c_str());
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::string error = importer.GetErrorString();
		LOG_ERROR("Cant load mesh file, Error: {}", error);
		return {};
	}

	MeshDecodeData decodeData;
	ProcessAssimpNode(decodeData, scene->mRootNode, scene);

	return decodeData;
}

void MeshDecoder::ProcessAssimpNode(MeshDecodeData& DecodeData, aiNode* AiNode, const aiScene* AiScene)
{
	if (AiNode->mNumMeshes > 0)
	{
		MeshDecodeData::Mesh mesh;
		uint32 indexOffset = 0;
		uint32 vertexOffset = 0;
		std::vector<Vertex> vertices;
		std::vector<uint32> indices;

		mesh.submeshes.resize(AiNode->mNumMeshes);
		
		//This represent ONE Mesh
		for (uint32 i = 0; i < AiNode->mNumMeshes; i++)
		{
			aiMesh* aiMesh = AiScene->mMeshes[AiNode->mMeshes[i]];
			mesh.submeshes[i] = DecodeAiMesh(aiMesh, indexOffset, vertexOffset, indices, vertices);
		}

		mesh.indices = indices;
		mesh.vertices = vertices;

		DecodeData.meshes.push_back(mesh);
	}

	//These are new meshes for rendering but in the hierarchy they are children
	for (uint32 i = 0; i < AiNode->mNumChildren; i++)
	{
		ProcessAssimpNode(DecodeData, AiNode->mChildren[i], AiScene);
	}
}

MeshDecodeData::Mesh::SubMesh MeshDecoder::DecodeAiMesh(aiMesh* AiMesh, uint32& IndexOffset, uint32& VertexOffset, Vector<uint32>& Indices, Vector<Vertex>& Vertices)
{
	for (uint32 i = 0; i < AiMesh->mNumVertices; i++)
	{
		Vertex vertex;
		aiVector3D aiVertex = AiMesh->mVertices[i];

		vertex.position = Vector4(aiVertex.x, aiVertex.y, aiVertex.z, 1);

		if (AiMesh->HasNormals())
		{
			aiVector3D aiNormals = AiMesh->mNormals[i];
			vertex.normal = Vector3(aiNormals.x, aiNormals.y, aiNormals.z);
		}

		if (AiMesh->mTextureCoords[0])
		{
			aiVector3D aiUV = AiMesh->mTextureCoords[0][i];
			vertex.uv = Vector2(aiUV.x,aiUV.y);
		}

		if (AiMesh->HasTangentsAndBitangents())
		{
			aiVector3D aiTangent = AiMesh->mTangents[i];
			aiVector3D aiBiTangent = AiMesh->mBitangents[i];
			vertex.tangent = Vector3(aiTangent.x, aiTangent.y, aiTangent.z);
			vertex.bitangent = Vector3(aiBiTangent.x, aiBiTangent.y, aiBiTangent.z);
		}

		Vertices.push_back(vertex);
	}

	//We know it will always be a max of 3 indcies per face as we us aiProcess_Triangulate when importing the scene
	size_t initalSize = Indices.size();
	Indices.resize((initalSize + (AiMesh->mNumFaces * 3)));

	std::for_each(std::execution::par, AiMesh->mFaces, AiMesh->mFaces + AiMesh->mNumFaces,
		[&](const aiFace& face) {
			size_t faceIndex = &face - AiMesh->mFaces;
			size_t globalOffset = initalSize + (faceIndex * 3);

			Indices[globalOffset] = face.mIndices[0] + VertexOffset;
			Indices[globalOffset + 1] = face.mIndices[1] + VertexOffset;
			Indices[globalOffset + 2] = face.mIndices[2] + VertexOffset;
		});

	//std::shared_ptr<Material> material;
	//if (AiMesh->mMaterialIndex >= 0)
	//{
	//	//aiMaterial* assimpMaterial = aScene->mMaterials[AiMesh->mMaterialIndex];
	//	//material = Material::CreateDefaultPolygon();
	//}

	MeshDecodeData::Mesh::SubMesh subMesh;
	subMesh.startIndex = IndexOffset;
	subMesh.indexCount = static_cast<uint32>(Indices.size() - IndexOffset);
	//subMesh.material = material;

	IndexOffset = static_cast<uint32>(Indices.size());
	VertexOffset += AiMesh->mNumVertices;

	return subMesh;
}
