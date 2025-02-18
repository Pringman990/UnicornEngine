#include "pch.h"
#include "DebugLineRenderer.h"	

DebugLineRenderer::DebugLineRenderer()
	:
	mShader(nullptr),
	mDebugVertexBuffer(nullptr),
	mMaxLines(1000)
{
}

DebugLineRenderer::~DebugLineRenderer()
{
}

void DebugLineRenderer::DrawLine(const Vector3& aStart, const Vector3& aEnd, const Color& aColor)
{
	if (mLineVertices.size() >= mMaxLines * 2)
	{
		uint32 newCapacity = mMaxLines * 2;
		Create(newCapacity);
	}
	mLineVertices.push_back({aStart, aColor});
	mLineVertices.push_back({ aEnd, aColor});
}

void DebugLineRenderer::DrawCube(const Vector3& aCenter, const Vector3& aSize, const Color& aColor)
{
	Vector3 halfSize = aSize *0.5f;

	Vector3 cubeVertices[8] = {
	 {aCenter.x - halfSize.x, aCenter.y + halfSize.y, aCenter.z - halfSize.z}, // v0
	 {aCenter.x + halfSize.x, aCenter.y + halfSize.y, aCenter.z - halfSize.z}, // v1
	 {aCenter.x - halfSize.x, aCenter.y - halfSize.y, aCenter.z - halfSize.z}, // v2
	 {aCenter.x + halfSize.x, aCenter.y - halfSize.y, aCenter.z - halfSize.z}, // v3
	 {aCenter.x - halfSize.x, aCenter.y + halfSize.y, aCenter.z + halfSize.z}, // v4
	 {aCenter.x + halfSize.x, aCenter.y + halfSize.y, aCenter.z + halfSize.z}, // v5
	 {aCenter.x - halfSize.x, aCenter.y - halfSize.y, aCenter.z + halfSize.z}, // v6
	 {aCenter.x + halfSize.x, aCenter.y - halfSize.y, aCenter.z + halfSize.z}  // v7
	};

	int cubeEdges[12][2] = {
	{0, 1}, {1, 3}, {3, 2}, {2, 0}, // Bottom face
	{4, 5}, {5, 7}, {7, 6}, {6, 4}, // Top face
	{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
	};

	for (int i = 0; i < 12; i++) {
		DrawLine(cubeVertices[cubeEdges[i][0]], cubeVertices[cubeEdges[i][1]], aColor);
	}
}

void DebugLineRenderer::Init()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mShader = Shader::Create("../../Binaries/Shaders/Debug_Line_VS.cso","../../Binaries/Shaders/Debug_Line_PS.cso", layout, 2);

	Create(mMaxLines);
}

void DebugLineRenderer::Create(uint32 aLineCapacity)
{
	if (mDebugVertexBuffer)
		mDebugVertexBuffer->Release();

	mMaxLines = aLineCapacity;

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(DebugLineVertex) * mMaxLines * 2;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, mDebugVertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		_LOG_RENDERER_ERROR("Failed to init Line renderer, {}", hr);
		return;
	}
}

void DebugLineRenderer::UpdateBuffer()
{
	if (mLineVertices.empty()) return;

	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(mDebugVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr)) {
		memcpy(mappedResource.pData, mLineVertices.data(), sizeof(DebugLineVertex) * mLineVertices.size());
		context->Unmap(mDebugVertexBuffer.Get(), 0);
	}
}

void DebugLineRenderer::Render()
{
	if (mLineVertices.empty()) return;
	//Renderer::GetInstance()->DisableDepthWriting();
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();

	UINT stride = sizeof(DebugLineVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mDebugVertexBuffer.GetAddressOf(), &stride, &offset);
	mShader->Bind(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	context->Draw(static_cast<UINT>(mLineVertices.size()), 0);
	Renderer::GetInstance()->AddDrawCall();

	//Renderer::GetInstance()->EnableDepthWriting();
	mLineVertices.clear();
}
