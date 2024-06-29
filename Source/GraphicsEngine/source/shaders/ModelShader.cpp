#include "GraphicsPch.h"
#include "ModelShader.h"

#include "../model/Model.h"
#include "../ConstantBuffer.h"

ModelShader::ModelShader()
{
}

ModelShader::~ModelShader()
{
}

void ModelShader::Render(const MeshData& someMeshData, const Transform& aTransform)
{
	ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetDX11()->GetDeviceContext();
	if (!PrepareRender() || !context || !mObjectBuffer)
	{
		std::cout << "Failed to draw Cube shader" << std::endl;
		return;
	}

	ObjectConstantBufferData bufferData;
	bufferData.modelToWorld = aTransform.GetMatrix();
	mObjectBuffer->Update(&bufferData);
	mObjectBuffer->Bind(ConstantBuffer::eObjectConstantBuffer);

	uint32_t stride = sizeof(Vertex);
	uint32_t offset = 0;
	context->IASetVertexBuffers(0, 1, &someMeshData.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(someMeshData.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(someMeshData.indexCount, 0, 0);
}

bool ModelShader::Init()
{
	ObjectConstantBufferData bufferData;
	bufferData.modelToWorld = Matrix();
	mObjectBuffer = std::make_unique<ConstantBuffer>(); 
	if (!mObjectBuffer->Init(sizeof(ObjectConstantBufferData), &bufferData))
		return false;

	return true;
}

bool ModelShader::CreateInputLayout(ID3DBlob* aVertexBlob)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT result = GraphicsEngine::GetInstance().GetDX11()->GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), aVertexBlob->GetBufferPointer(), aVertexBlob->GetBufferSize(), &mInputLayout);
	if (FAILED(result))
	{
		_com_error err(result);
		LPCTSTR errorMessage = err.ErrorMessage();
		std::cout << "Failed to create Shader: " << errorMessage << std::endl;
		return false;
	}

    return true;
}
