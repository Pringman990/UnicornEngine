#include "pch.h"
#include "Mesh.h"

#include "Shader.h"

Mesh::Mesh()
	:
	mTransform(Matrix())
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
	
	renderer->UpdateObjectBuffer(mTransform);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	deviceContext->PSSetShader(mShader->GetPixelShader(), nullptr, 0);
	deviceContext->VSSetShader(mShader->GetVertexShader(), nullptr, 0);
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(mShader->GetInputLayout());
	
	deviceContext->DrawIndexed(mIndexCount, 0, 0);
}

std::shared_ptr<Mesh> Mesh::Create(std::vector<Vertex>& someVertices, std::vector<uint32>& someIndices)
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D11Device* device = renderer->GetDevice();

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
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
		mesh->mIndexCount = (int)someIndices.size();
		HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexData, &mesh->mIndexBuffer);
		if (FAILED(result))
		{
			return nullptr;
		}
	}

	mesh->mShader = Shader::CreateDefault();

	return mesh;
}

std::shared_ptr<Mesh> Mesh::Create(
	std::vector<Vertex>& someVertices,
	std::vector<uint32>& someIndices, 
	const std::string& aVSPath, 
	const std::string& aPSPath
)
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D11Device* device = renderer->GetDevice();

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
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
		mesh->mIndexCount = (int)someIndices.size();
		HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexData, &mesh->mIndexBuffer);
		if (FAILED(result))
		{
			return nullptr;
		}
	}

	mesh->mShader = Shader::Create(aVSPath, aPSPath);

	return mesh;
}

Matrix& Mesh::GetMatrix()
{
	return mTransform;
}
