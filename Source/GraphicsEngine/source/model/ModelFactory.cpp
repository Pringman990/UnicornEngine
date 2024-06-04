#include "GraphicsPch.h"
#include "ModelFactory.h"

#include "Model.h"
#include "ModelInstance.h"

#include <FBXImporter/source/Importer.h>
#include "../GraphicsEngine/source/DX11.h"
#include "../GraphicsEngine/source/Material.h"

ModelFactory::ModelFactory()
{
	TGA::FBX::Importer::InitImporter();
}

ModelFactory::~ModelFactory()
{
	TGA::FBX::Importer::UninitImporter();
}

std::shared_ptr<Model> ModelFactory::GetModel(const std::wstring& aPath)
{
	auto it = mLoadedModels.find(aPath);
	if (it != mLoadedModels.end())
		return it->second;

	return LoadModelW(aPath);
}

ModelInstance ModelFactory::GetModelInstance(const std::wstring& aPath)
{
	std::shared_ptr<Model> model = GetModel(aPath);

	ModelInstance mdi;
	mdi.Init(model);

	return mdi;
}

std::shared_ptr<Model> ModelFactory::LoadModelW(const std::wstring& aPath)
{
	std::string pathA = WStringToString(aPath);

	TGA::FBX::Mesh tgaMesh = {};
	if (!TGA::FBX::Importer::LoadMeshA(pathA, tgaMesh))
		return nullptr;

	std::vector<MeshData> mdlMeshData;
	mdlMeshData.resize(tgaMesh.Elements.size());

	for (int v = 0; v < tgaMesh.Elements.size(); v++)
	{
		TGA::FBX::Mesh::Element& element = tgaMesh.Elements[v];

		MeshData& meshData = mdlMeshData[v];
		std::vector<Vertex> mdlVertices;
		mdlVertices.resize(element.Vertices.size());

		for (size_t i = 0; i < element.Vertices.size(); i++)
		{
			mdlVertices[i].position = {
				element.Vertices[i].Position[0],
				element.Vertices[i].Position[1],
				element.Vertices[i].Position[2],
				element.Vertices[i].Position[3]
			};

			/*	for (int vCol = 0; vCol < 4; vCol++)
				{
					mdlVertices[i].color = {
						element.Vertices[i].VertexColors[vCol][0],
						element.Vertices[i].VertexColors[vCol][1],
						element.Vertices[i].VertexColors[vCol][2],
						element.Vertices[i].VertexColors[vCol][3]
					};

					if (element.Vertices[i].VertexColors[vCol][0] +
						element.Vertices[i].VertexColors[vCol][1] +
						element.Vertices[i].VertexColors[vCol][2] +
						element.Vertices[i].VertexColors[vCol][3] == 0)
					{
						mdlVertices[i].color = { 1,1,1,1 };
					}
				}*/

			mdlVertices[i].normal = {
				element.Vertices[i].Normal[0],
				element.Vertices[i].Normal[1],
				element.Vertices[i].Normal[2]
			};

			mdlVertices[i].bitangent = {
				element.Vertices[i].BiNormal[0],
				element.Vertices[i].BiNormal[1],
				element.Vertices[i].BiNormal[2]
			};

			mdlVertices[i].tangent = {
				element.Vertices[i].Tangent[0],
				element.Vertices[i].Tangent[1],
				element.Vertices[i].Tangent[2]
			};

			//for (uint32_t uv = 0; uv < 4; uv++)
			{
				mdlVertices[i].uv = {
					element.Vertices[i].UVs[0][0],
					element.Vertices[i].UVs[0][1]
				};
			}

			std::vector<uint32_t>& mdlIndices = element.Indices;

			HRESULT result;

			D3D11_BUFFER_DESC vertexBufferDesc{};
			vertexBufferDesc.ByteWidth = static_cast<UINT>(mdlVertices.size()) * static_cast<UINT>(sizeof(Vertex));
			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
			vertexSubresourceData.pSysMem = &mdlVertices[0];

			ID3D11Buffer* vertexBuffer;
			result = GraphicsEngine::GetInstance().GetDX11().GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);
			if (FAILED(result))
			{
				return nullptr;
			}

			D3D11_BUFFER_DESC indexBufferDesc{};
			indexBufferDesc.ByteWidth = static_cast<UINT>(mdlIndices.size()) * static_cast<UINT>(sizeof(float)); // TODO: What :P Sizeof should be uint.
			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA indexSubresourceData{};
			indexSubresourceData.pSysMem = &mdlIndices[0];

			ID3D11Buffer* indexBuffer;
			result = GraphicsEngine::GetInstance().GetDX11().GetDevice()->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);
			if (FAILED(result))
			{
				return nullptr;
			}

			meshData.vertexCount = static_cast<uint32_t>(mdlVertices.size());
			meshData.indexCount = static_cast<uint32_t>(mdlIndices.size());
			meshData.vertexBuffer = vertexBuffer;
			meshData.indexBuffer = indexBuffer;
			Material material;
			material.CreateMaterial("", "", "");
			meshData.material = std::make_shared<Material>(material);
		}

	}
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->Init(mdlMeshData);
	mLoadedModels.insert({ aPath, model });
	return model;
}
