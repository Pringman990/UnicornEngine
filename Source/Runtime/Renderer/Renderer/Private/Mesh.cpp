#include "pch.h"
#include "Mesh.h"

#include "Shader.h"

Mesh::Mesh()
	:
	mTransform()
{
}

Mesh::~Mesh()
{
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();
}

void Mesh::Draw()
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D11DeviceContext* deviceContext = renderer->GetDeviceContext();

	Vector3 pos = mTransform.GetPosition();
	Vector3 halfSize = mTransform.GetScale() / 2;
	Vector3 maxBounds = pos + halfSize;
	Vector3 minBounds = pos - halfSize;

	renderer->UpdateObjectBuffer(mTransform.GetMatrix(), minBounds, maxBounds);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int32 i = 0; i < mSubMeshes.size(); i++)
	{
		SubMesh& subMesh = mSubMeshes[i];
		subMesh.material->Bind();
		deviceContext->DrawIndexed(subMesh.indexCount, subMesh.startIndex, 0);
	}

	//deviceContext->PSSetShader(mShader->GetPixelShader(), nullptr, 0);
	//deviceContext->VSSetShader(mShader->GetVertexShader(), nullptr, 0);
	//deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//deviceContext->IASetInputLayout(mShader->GetInputLayout());

}

void Mesh::SetMaterial(uint32 aSubMeshIndex, std::shared_ptr<Material>& aMaterial)
{
	if (aSubMeshIndex >= mSubMeshes.size())
	{
		_LOG_RENDERER_WARNING("Sub mesh index out of range at: {},");
		return;
	}

	mSubMeshes[aSubMeshIndex].material = aMaterial;
}

std::shared_ptr<Material> Mesh::GetMaterial(uint32 aSubMeshIndex)
{
	if (aSubMeshIndex >= mSubMeshes.size())
	{
		_LOG_RENDERER_WARNING("Sub mesh index out of range at: {},");
		return nullptr;
	}

	return mSubMeshes[aSubMeshIndex].material;
}

std::shared_ptr<Mesh> Mesh::CreateCube()
{
	std::vector<Vertex> vertices =
	{
		{{ -0.5f, 0.5f, -0.5f, 1}},

		{{ 0.5f, 0.5f, -0.5f, 1}},

		{ {-0.5f, -0.5f, -0.5f, 1 } },

		{ { 0.5f, -0.5f, -0.5f , 1 }},

		{ { -0.5f, 0.5f, 0.5f , 1 }},

		{ { 0.5f, 0.5f, 0.5f , 1  }},

		{ { -0.5f, -0.5f, 0.5f , 1 }},

		{ {0.5f, -0.5f, 0.5f , 1 }},
	};

	std::vector<uint32> indices =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
	};

	std::shared_ptr<Mesh> mesh = Create(vertices, indices);
	auto material = Material::CreateDefaultVoxel();
	mesh->SetMaterial(0, material);

	return mesh;
}

std::shared_ptr<Mesh> Mesh::CreateSphereInvertedNormals()
{

	return std::shared_ptr<Mesh>();
}

std::shared_ptr<Mesh> Mesh::Create(std::vector<Vertex>& someVertices, std::vector<uint32>& someIndices)
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D11Device* device = renderer->GetDevice();

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	SubMesh subMesh;
	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * (UINT)someVertices.size();
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = someVertices.data();
		HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mesh->mVertexBuffer);
		if (FAILED(result))
		{
			return nullptr;
		}
	}
	{
		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(uint32) * (UINT)someIndices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = someIndices.data();
		
		subMesh.startIndex = 0;
		subMesh.indexCount = (int)someIndices.size();
		
		HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexData, &mesh->mIndexBuffer);
		if (FAILED(result))
		{
			return nullptr;
		}
	}
	mesh->mSubMeshes.push_back(subMesh);
	return mesh;
}

Transform& Mesh::GetTransform() 
{
	return mTransform;
}
