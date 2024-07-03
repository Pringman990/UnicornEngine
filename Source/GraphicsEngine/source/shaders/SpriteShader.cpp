#include "GraphicsPch.h"
#include "SpriteShader.h"

#include "ConstantBuffer.h"

SpriteShader::SpriteShader()
	:
	mVertexCount(0),
	mIndexCount(0),
	mVertexBuffer(nullptr),
	mIndexBuffer(nullptr)
{
}

SpriteShader::~SpriteShader()
{
}

void SpriteShader::Render(const Transform& aTransform)
{
	ID3D11DeviceContext* context = Engine::GetGraphicsEngine().GetDX11()->GetDeviceContext();
	if (!PrepareRender() || !context)
	{
		std::cout << "Failed to draw Sprite shader" << std::endl;
		return;
	}

	ObjectConstantBufferData bufferData;
	bufferData.modelToWorld = aTransform.GetMatrix();
	mObjectBuffer->Update(&bufferData);
	mObjectBuffer->Bind(ConstantBuffer::eObjectConstantBuffer);

	uint32_t stride = sizeof(Vertex); 
	uint32_t offset = 0; 
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(mIndexCount, 0, 0);
}

bool SpriteShader::Init()
{
	Vertex vertices[] = 
	{
		{{-0.5f,  0.5f, 0.0f, 1.0f}, {1, 1, 1, 1}, {0.0f, 0.0f}}, // Top-left
		{{ 0.5f,  0.5f, 0.0f, 1.0f}, {1, 1, 1, 1}, {1.0f, 0.0f}}, // Top-right
		{{ 0.5f, -0.5f, 0.0f, 1.0f}, {1, 1, 1, 1}, {1.0f, 1.0f}}, // Bottom-right
		{{-0.5f, -0.5f, 0.0f, 1.0f}, {1, 1, 1, 1}, {0.0f, 1.0f}}  // Bottom-left
	};

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; 
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = vertices;

	HRESULT result = Engine::GetGraphicsEngine().GetDX11()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &mVertexBuffer);
	if (FAILED(result)) 
	{
		return false;
	}

	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = indices;

	result = Engine::GetGraphicsEngine().GetDX11()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &mIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	mVertexCount = ARRAYSIZE(vertices); 
	mIndexCount = ARRAYSIZE(indices);

	ObjectConstantBufferData bufferData;
	bufferData.modelToWorld = Matrix();
	mObjectBuffer = std::make_unique<ConstantBuffer>();
	if (!mObjectBuffer->Init(sizeof(ObjectConstantBufferData), &bufferData))
		return false;

    return true;
}

bool SpriteShader::CreateInputLayout(ID3DBlob* aVertexBlob)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT result = Engine::GetGraphicsEngine().GetDX11()->GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), aVertexBlob->GetBufferPointer(), aVertexBlob->GetBufferSize(), &mInputLayout);
	if (FAILED(result))
	{
		_com_error err(result);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::wcout << "Failed to create Shader: " << errorMessage << std::endl;
		return false;
	}

	return true;
}
