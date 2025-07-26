#include "MeshLoader.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

MeshLoader::MeshLoader()
{
}

MeshLoader::~MeshLoader()
{
}

AssetLoadResult<Mesh> MeshLoader::Load(const String& VirtualPath, Mesh* Asset)
{
	Assimp::Importer importer;

	const uint32 readFlags =
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	ByteBuffer buffer = FileSystem::ReadAll(VirtualPath);

	//const aiScene* scene = importer.ReadFile(aFilePath.c_str(), readFlags);
	const aiScene* scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), readFlags, "fbx");
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::string error = importer.GetErrorString();
		_LOG_RENDERER_ERROR("Cant load file, Error: {}", error);
		return AssetLoadResult<Mesh>();
	}

	//someMesh.reserve(scene->mNumMeshes);

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

	AssetLoadResult<Mesh> result;

	ProcessAiNodes(scene->mRootNode, scene, result);

	return result;
}

bool MeshLoader::Save(const String& VirtualPath, AssetHandle<Mesh> Handle)
{
	return false;
}

void MeshLoader::ProcessAiNodes(aiNode* Node, const aiScene* Scene, AssetLoadResult<Mesh>& Result)
{
	if (Node->mNumMeshes > 0)
	{
		uint32 indexOffset = 0;
		Vector<Vertex3D> vertices;
		Vector<uint32> indices;

		//This represent ONE Mesh
		//for (uint32 i = 0; i < aNode->mNumMeshes; i++)
		{
			//We only take the first mesh found
			aiMesh* aiMesh = Scene->mMeshes[Node->mMeshes[0]];
			ConvertAiMeshToMesh(aiMesh, Scene, Result, indexOffset, vertices, indices);
		}

		Result.metadata.vertexCount = static_cast<uint32>(vertices.size());

		//TODO: Change this if supporting threaded loading
		CommandPool* pool = Renderer::Get()->GetCurrentFrameSyncCommandPool();
		Result.resourceData.mIndexBuffer = IndexBuffer::Create(indices, pool);
		Result.resourceData.mVertexBuffer = VertexBuffer::Create(vertices, pool);

		//someMesh.emplace_back(mesh);
	}

	//These are new meshes for rendering but in the hierarchy they are children
	for (uint32 i = 0; i < Node->mNumChildren; i++)
	{
		ProcessAiNodes(Node->mChildren[i], Scene, Result);
	}
}

void MeshLoader::ConvertAiMeshToMesh(aiMesh* AiMesh, const aiScene* Scene, AssetLoadResult<Mesh>& Result, uint32& IndexOffset, Vector<Vertex3D>& Vertices, Vector<uint32>& Indices)
{
	for (uint32 i = 0; i < AiMesh->mNumVertices; i++)
	{
		Vertex3D vertex;
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
			vertex.uv = Vector2(aiUV.x, aiUV.y);
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
	//Indices.resize(IndexOffset + (AiMesh->mNumFaces * 3));

	//std::for_each(std::execution::par, aAiMesh->mFaces, aAiMesh->mFaces + aAiMesh->mNumFaces,
	//	[&](const aiFace& face) {
	//		size_t faceIndex = &face - aAiMesh->mFaces;
	//		size_t globalOffset = aIndexOffset + (faceIndex * 3);

	//		someIndices[globalOffset] = face.mIndices[0];
	//		someIndices[globalOffset + 1] = face.mIndices[1];
	//		someIndices[globalOffset + 2] = face.mIndices[2];
	//	});

	for (UINT i = 0; i < AiMesh->mNumFaces; i++) {
		aiFace face = AiMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			Indices.push_back(face.mIndices[j] + IndexOffset);
	}

	//Material* material = nullptr;
	//if (aAiMesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* assimpMaterial = aScene->mMaterials[aAiMesh->mMaterialIndex];
	//	material = LoadMaterial(assimpMaterial);
	//}

	Mesh::SubMesh subMesh;
	subMesh.indexOffset = IndexOffset;
	subMesh.indexCount = static_cast<uint32>(Indices.size() - IndexOffset);
	//subMesh.material = material;

	Result.resourceData.mSubMeshes.push_back(subMesh);

	IndexOffset += static_cast<uint32>(AiMesh->mNumVertices);
}
