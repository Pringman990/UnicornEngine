#include "Factories/MeshFactory.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

const Vector<GPUResourceHandle<GPUMesh>> MeshFactory::CreateMeshes(ByteBuffer& MeshData)
{
	Assimp::Importer importer;

	const aiScene* scene = GetScene(importer, MeshData);
	if (scene == nullptr)
	{
		_LOG_RENDERER_ERROR("Failed to load Assimp scene");
		return {};
	}

	Vector<GPUResourceHandle<GPUMesh>> meshes;
	ProcessAiNodes(scene->mRootNode, scene, meshes, "");

	return meshes;
}

const GPUResourceHandle<GPUMesh> MeshFactory::CreateMesh(ByteBuffer& MeshData, const String& MeshName)
{
	Assimp::Importer importer;

	const aiScene* scene = GetScene(importer, MeshData);
	if (scene == nullptr)
	{
		_LOG_RENDERER_ERROR("Failed to load Assimp scene");
		return {};
	}

	aiNode* node = GetNodeByName(scene->mRootNode, MeshName);
	if (node == nullptr)
	{
		_LOG_RENDERER_ERROR("Did not find mesh with name: {}", MeshName);
		return GPUResourceHandle<GPUMesh>::Invalid();
	}

	uint32 indexOffset = 0;
	Vector<Vertex3D> vertices;
	Vector<uint32> indices;

	Vector<GPUMesh::GPUSubMesh> submMeshes;

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
		ConvertAiMeshToMesh(aiMesh, scene, indexOffset, vertices, indices, submMeshes);
	}

	Renderer* renderer = Renderer::Get();
	auto& gpuManager = renderer->GetGPUResourceManager();

	GPUResourceHandle<GPUMesh> meshHandle = gpuManager.AllocateResource<GPUMesh>();
	GPUMesh* mesh = gpuManager.GetResource(meshHandle);

	mesh->indexBuffer = IndexBuffer::Create(indices, renderer->GetCurrentFrameSyncCommandPool());
	mesh->vertexBuffer = VertexBuffer::Create(vertices, renderer->GetCurrentFrameSyncCommandPool());
	mesh->subMeshes = submMeshes;

	return meshHandle;
}

const aiScene* MeshFactory::GetScene(Assimp::Importer& Importer, ByteBuffer& MeshData)
{
	const uint32 readFlags =
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	const aiScene* scene = Importer.ReadFileFromMemory(MeshData.data(), MeshData.size(), readFlags, "fbx");
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::string error = Importer.GetErrorString();
		_LOG_RENDERER_ERROR("Cant load file, Error: {}", error);
		return nullptr;
	}

	return scene;
}

aiNode* MeshFactory::GetNodeByName(aiNode* Node, const String& NodeName)
{
	if (Node->mName.C_Str() == NodeName)
	{
		return Node;
	}

	for (uint32 i = 0; i < Node->mNumChildren; i++)
	{
		aiNode* found = GetNodeByName(Node->mChildren[i], NodeName);
		if (found != nullptr)
		{
			return found;
		}
	}

	return nullptr;
}

void MeshFactory::ProcessAiNodes(aiNode* Node, const aiScene* Scene, Vector<GPUResourceHandle<GPUMesh>>& Meshes, const String& MeshName)
{
	uint32 indexOffset = 0;
	Vector<Vertex3D> vertices;
	Vector<uint32> indices;
	Vector<GPUMesh::GPUSubMesh> submMeshes;
	if (MeshName.empty())
	{
		
		for (uint32 i = 0; i < Node->mNumMeshes; i++)
		{
			aiMesh* aiMesh = Scene->mMeshes[Node->mMeshes[i]];
			ConvertAiMeshToMesh(aiMesh, Scene, indexOffset, vertices, indices, submMeshes);
		}
	}
	else
	{
		if (Node->mName.C_Str() == MeshName)
		{
			for (uint32 i = 0; i < Node->mNumMeshes; i++)
			{
				aiMesh* aiMesh = Scene->mMeshes[Node->mMeshes[i]];
				ConvertAiMeshToMesh(aiMesh, Scene, indexOffset, vertices, indices, submMeshes);
			}

			return;
		}
	}

	for (uint32 i = 0; i < Node->mNumChildren; i++)
	{
		ProcessAiNodes(Node->mChildren[i], Scene, Meshes, MeshName);
	}
}

void MeshFactory::ConvertAiMeshToMesh(
	aiMesh* AiMesh,
	const aiScene* Scene,
	uint32& IndexOffset, 
	Vector<Vertex3D>& Vertices,
	Vector<uint32>& Indices,
	Vector<GPUMesh::GPUSubMesh>& SubMeshes
)
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

	for (UINT i = 0; i < AiMesh->mNumFaces; i++) 
	{
		aiFace face = AiMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			Indices.push_back(face.mIndices[j] + IndexOffset);
		}
	}

	GPUMesh::GPUSubMesh subMesh;
	subMesh.indexCount = static_cast<uint32>(Indices.size() - IndexOffset);
	subMesh.indexOffset = IndexOffset;

	SubMeshes.push_back(subMesh);

	IndexOffset += static_cast<uint32>(AiMesh->mNumVertices);
}
