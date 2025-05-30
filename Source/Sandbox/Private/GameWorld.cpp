#include "GameWorld.h"

#include <StandardTypes/StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <Logger/Logger.h>

#include <RendererCore.h>
#include <d3dcompiler.h>

namespace {
	struct alignas(256) MVPConstants
	{
		Matrix mvpMatrix;
		Matrix pad0;
		Matrix pad1;
		Matrix pad2;
	};

	GameWorld gameworld;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;
	ComPtr<ID3D12Resource> constantBuffer;
	
	MVPConstants* cbData = nullptr;
	Matrix modelMatrix;

	RenderTarget* rendertarget;

	ComPtr<ID3D12Resource> computeOutputTexture;
	ComPtr<ID3DBlob> csBlob;
	ComPtr<ID3D12RootSignature> computeRootSignature;
	ComPtr<ID3D12PipelineState> computePipelineState;

	ComPtr<ID3D12RootSignature> fullscreenRootSignature;
	ComPtr<ID3D12PipelineState> fullscreenPSO;
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE uavGPUHandle;

	Vertex verticies[] =
	{
		// Front face
	{ { -1, -1, -1, 1 }, { 1, 0, 0 } }, // Bottom-left-front (red)
	{ { -1,  1, -1, 1 }, { 1, 0, 0 } }, // Top-left-front
	{ {  1,  1, -1, 1 }, { 1, 0, 0 } }, // Top-right-front
	{ {  1, -1, -1, 1 }, { 1, 0, 0 } }, // Bottom-right-front

	// Back face
	{ { -1, -1,  1, 1 }, { 0, 1, 0 } }, // Bottom-left-back (green)
	{ {  1, -1,  1, 1 }, { 0, 1, 0 } }, // Bottom-right-back
	{ {  1,  1,  1, 1 }, { 0, 1, 0 } }, // Top-right-back
	{ { -1,  1,  1, 1 }, { 0, 1, 0 } }, // Top-left-back

	// Left face
	{ { -1, -1,  1, 1 }, { 0, 0, 1 } }, // Bottom-left-back (blue)
	{ { -1,  1,  1, 1 }, { 0, 0, 1 } }, // Top-left-back
	{ { -1,  1, -1, 1 }, { 0, 0, 1 } }, // Top-left-front
	{ { -1, -1, -1, 1 }, { 0, 0, 1 } }, // Bottom-left-front

	// Right face
	{ { 1, -1, -1, 1 }, { 1, 1, 0 } }, // Bottom-right-front (yellow)
	{ { 1,  1, -1, 1 }, { 1, 1, 0 } }, // Top-right-front
	{ { 1,  1,  1, 1 }, { 1, 1, 0 } }, // Top-right-back
	{ { 1, -1,  1, 1 }, { 1, 1, 0 } }, // Bottom-right-back

	// Top face
	{ { -1, 1, -1, 1 }, { 1, 0, 1 } }, // Top-left-front (magenta)
	{ { -1, 1,  1, 1 }, { 1, 0, 1 } }, // Top-left-back
	{ {  1, 1,  1, 1 }, { 1, 0, 1 } }, // Top-right-back
	{ {  1, 1, -1, 1 }, { 1, 0, 1 } }, // Top-right-front

	// Bottom face
	{ { -1, -1, -1, 1 }, { 0, 1, 1 } }, // Bottom-left-front (cyan)
	{ {  1, -1, -1, 1 }, { 0, 1, 1 } }, // Bottom-right-front
	{ {  1, -1,  1, 1 }, { 0, 1, 1 } }, // Bottom-right-back
	{ { -1, -1,  1, 1 }, { 0, 1, 1 } }, // Bottom-left-back
	};

	uint16_t cubeIndices[] = {
		// Front face
		0, 1, 2,
		0, 2, 3,

		// Back face
		4, 5, 6,
		4, 6, 7,

		// Left face
		8, 9, 10,
		8, 10, 11,

		// Right face
		12, 13, 14,
		12, 14, 15,

		// Top face
		16, 17, 18,
		16, 18, 19,

		// Bottom face
		20, 21, 22,
		20, 22, 23,
	};
}



GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	LOG_INFO("GameWorld init...");

	float aspect = (16.f / 9.f);
	mCamera.SetPerspective(90, aspect, 0.01f, 1000.f);
	mCamera.GetTransform().SetPosition({ 0,0,-5 });
	Renderer::GetInstance()->SetMainCamera(&mCamera);


	const uint32 vertexBufferSize = sizeof(verticies);

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

	HRESULT hr = Renderer::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, 
		D3D12_HEAP_FLAG_NONE, 
		&vbufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(vertexBuffer.GetAddressOf())
	);
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}

	void* vdata = nullptr;
	vertexBuffer->Map(0, nullptr, &vdata);
	memcpy(vdata, verticies, vertexBufferSize);
	vertexBuffer->Unmap(0, nullptr);

	vbView = {};
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vbView.SizeInBytes = vertexBufferSize;
	vbView.StrideInBytes = sizeof(Vertex);

	const uint32 indexBufferSize = sizeof(cubeIndices);

	CD3DX12_RESOURCE_DESC ibufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

	hr = Renderer::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&ibufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indexBuffer.GetAddressOf())
	);
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}

	void* idata = nullptr;
	indexBuffer->Map(0, nullptr, &idata);
	memcpy(idata, cubeIndices, indexBufferSize);
	indexBuffer->Unmap(0, nullptr);

	ibView = {};
	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	ibView.SizeInBytes = indexBufferSize;
	ibView.Format = DXGI_FORMAT_R16_UINT;



	CD3DX12_RESOURCE_DESC cbufferDesc = CD3DX12_RESOURCE_DESC::Buffer(256); // Must be 256-byte aligned size

	hr = Renderer::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&cbufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer)
	);

	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create constant buffer\n");
	}


	hr = constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&cbData));
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to map constant buffer\n");
	}

	modelMatrix.Translation(Vector3(0, 0, 0));
	cbData->mvpMatrix = mCamera.GetClipSpaceMatrix() * modelMatrix;

	CD3DX12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].InitAsConstantBufferView(0);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}
	
	hr = Renderer::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}

	hr = D3DReadFileToBlob(L"../../Binaries/Shaders/Triangle_VS.cso", vsBlob.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}

	hr = D3DReadFileToBlob(L"../../Binaries/Shaders/Triangle_PS.cso", psBlob.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,                             D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3,            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // whatever format your RTV uses
	psoDesc.SampleDesc.Count = 1;

	hr = Renderer::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}
	auto windowInfo = Application::GetInstance()->GetApplication()->GetWindowInfo();
	rendertarget = RenderTarget::Create(Vector2(windowInfo.viewportWidth, windowInfo.viewportHeight));

	//InitCompute();
}

void GameWorld::InitCompute()
{
	auto device = Renderer::GetInstance()->GetDevice();
	auto windowInfo = Application::GetInstance()->GetApplication()->GetWindowInfo();

	DXGI_FORMAT computeOutputFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		computeOutputFormat, windowInfo.viewportWidth, windowInfo.viewportHeight, 1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&computeOutputTexture)
	);
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create vertex buffer\n");
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = computeOutputFormat;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Texture2D.PlaneSlice = 0;

	auto uavCPUDescHandle = Renderer::GetInstance()->GetSRVHeapManager().Allocate();
	device->CreateUnorderedAccessView(
		computeOutputTexture.Get(), nullptr,
		&uavDesc,
		uavCPUDescHandle // from descriptor heap
	);

	uavGPUHandle = Renderer::GetInstance()->GetSRVHeapManager().GetGPUHandleFromCPUHandle(uavCPUDescHandle);

	CD3DX12_DESCRIPTOR_RANGE range;
	range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_ROOT_PARAMETER param;
	param.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_ROOT_SIGNATURE_DESC computeRootSigDesc;
	computeRootSigDesc.Init(1, &param, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

	ComPtr<ID3DBlob> rootSigBlob;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&computeRootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, rootSigBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("Failed\n");
	}

	hr = Renderer::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(computeRootSignature.GetAddressOf()));
	if (FAILED(hr)) {
		OutputDebugStringA("Failed\n");
	}

	hr = D3DReadFileToBlob(L"../../Binaries/Shaders/Basic_CS.cso", csBlob.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("Failed\n");
	}

	D3D12_COMPUTE_PIPELINE_STATE_DESC cPipelineDesc = {};
	cPipelineDesc.CS = { csBlob->GetBufferPointer(), csBlob->GetBufferSize() };
	cPipelineDesc.pRootSignature = computeRootSignature.Get();
	cPipelineDesc.NodeMask = 0;
	cPipelineDesc.CachedPSO.CachedBlobSizeInBytes = 0;
	cPipelineDesc.CachedPSO.pCachedBlob = nullptr;
	cPipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	hr = Renderer::GetInstance()->GetDevice()->CreateComputePipelineState(&cPipelineDesc, IID_PPV_ARGS(computePipelineState.GetAddressOf()));
	if (FAILED(hr)) {
		_com_error err(hr);
		OutputDebugStringW(err.ErrorMessage());
	}

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = computeRootSignature.Get();
	psoDesc.CS = { csBlob->GetBufferPointer(), csBlob->GetBufferSize() };
	device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(computePipelineState.GetAddressOf()));





	//CD3DX12_DESCRIPTOR_RANGE rangeSRV1, rangeSampler1;
	//rangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);     // t0
	//rangeSampler1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0); // s0

	//CD3DX12_ROOT_PARAMETER params1[2];
	//params1[0].InitAsDescriptorTable(1, &rangeSRV1, D3D12_SHADER_VISIBILITY_PIXEL);
	//params1[1].InitAsDescriptorTable(1, &rangeSampler1, D3D12_SHADER_VISIBILITY_PIXEL);

	//CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc1;
	//rootSigDesc1.Init(_countof(params1), params1, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);


	//ComPtr<ID3DBlob> rootSigBlob1;
	//ComPtr<ID3DBlob> errorBlob1;
	//D3D12SerializeRootSignature(&rootSigDesc1, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob1, &errorBlob1);
	//if (errorBlob1) {
	//	OutputDebugStringA((char*)errorBlob1->GetBufferPointer());
	//}
	//device->CreateRootSignature(0, rootSigBlob1->GetBufferPointer(), rootSigBlob1->GetBufferSize(), IID_PPV_ARGS(&fullscreenRootSignature));


	ComPtr<ID3DBlob> vsBlob1, psBlob1;
	D3DReadFileToBlob(L"../../Binaries/Shaders/Full_Screen_Quad_VS.cso", &vsBlob1);
	D3DReadFileToBlob(L"../../Binaries/Shaders/Full_Screen_Quad_PS.cso", &psBlob1);

	assert(vsBlob1 && vsBlob1->GetBufferPointer());
	assert(psBlob1 && psBlob1->GetBufferPointer());

	D3D12_INPUT_LAYOUT_DESC inputLayout = {};
	inputLayout.pInputElementDescs = nullptr;
	inputLayout.NumElements = 0;

	fullscreenRootSignature = Renderer::GetInstance()->GetRootSignatureManager().Get("SpriteBindless");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
	pso.pRootSignature = fullscreenRootSignature.Get();
	pso.VS = { vsBlob1->GetBufferPointer(), vsBlob1->GetBufferSize() };
	pso.PS = { psBlob1->GetBufferPointer(), psBlob1->GetBufferSize() };
	pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pso.SampleMask = UINT_MAX;
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // must match swapchain format
	pso.SampleDesc.Count = 1;
	pso.SampleDesc.Quality = 0;
	pso.InputLayout = { nullptr, 0 }; // only if VS uses SV_VertexID
	pso.CachedPSO = {};

	hr = device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&fullscreenPSO));
	if (FAILED(hr)) {
		_com_error err(hr);
		OutputDebugStringW(err.ErrorMessage());
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	srvCPUHandle = Renderer::GetInstance()->GetSRVHeapManager().Allocate();
	device->CreateShaderResourceView(computeOutputTexture.Get(), &srvDesc, srvCPUHandle);

	srvGPUHandle = Renderer::GetInstance()->GetSRVHeapManager().GetGPUHandleFromCPUHandle(srvCPUHandle);
}

void GameWorld::Render()
{
	auto cmdList = Renderer::GetInstance()->GetMainCommandList();

	//cmdList->ClearRenderTargetView(rendertarget->GetRTVHandle(), Color(0.5f, 0.5f, 0.7f, 0), 0, nullptr);
	//cmdList->ClearDepthStencilView(rendertarget->GetDSVHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
	//
	//cmdList->OMSetRenderTargets(1, &rendertarget->GetRTVHandle(), false, &rendertarget->GetDSVHandle());

	cmdList->SetPipelineState(pipelineState.Get());
	cmdList->SetGraphicsRootSignature(rootSignature.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());

	cmdList->DrawIndexedInstanced(_countof(cubeIndices), 1, 0, 0, 0);

	UpdateCamera();

	cbData->mvpMatrix = mCamera.GetClipSpaceMatrix() * modelMatrix;

	constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&cbData));

	//cmdList->SetComputeRootSignature(computeRootSignature.Get());
	//cmdList->SetPipelineState(computePipelineState.Get());

	//cmdList->SetComputeRootDescriptorTable(0, uavGPUHandle);
	//auto windowInfo = Application::GetInstance()->GetApplication()->GetWindowInfo();
	//cmdList->SetComputeRootUnorderedAccessView(0, computeOutputTexture->GetGPUVirtualAddress());
	//cmdList->Dispatch((windowInfo.viewportWidth + 7) / 8, (windowInfo.viewportHeight + 7) / 8, 1);

	//Renderer::GetInstance()->RenderToBackbuffer();
	//
	//cmdList->SetPipelineState(fullscreenPSO.Get());
	//cmdList->SetGraphicsRootSignature(fullscreenRootSignature.Get());
	//cmdList->SetGraphicsRootDescriptorTable(0, rendertarget->GetTexture()->GetGPUHandle());
	//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//cmdList->DrawInstanced(3, 1, 0, 0);
}

void GameWorld::UpdateCamera()
{
	if (!GetAsyncKeyState(VK_RBUTTON))
	{
		InputMapper::GetInstance()->ReleaseMouse();
		return;
	}

	Transform& transform = mCamera.GetTransform();
	Matrix matrix = transform.GetMatrix();
	Vector3 position = transform.GetPosition();
	Vector3 right = matrix.Right();
	Vector3 up = matrix.Up();
	Vector3 forward = matrix.Forward();

	float realMovementSpeed = 5 * Timer::GetInstance()->GetDeltaTime();

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		realMovementSpeed = 10 * Timer::GetInstance()->GetDeltaTime();
	}

	if (GetAsyncKeyState('W'))
	{
		position = position + (forward * -realMovementSpeed);
	}
	if (GetAsyncKeyState('S'))
	{
		position = position + (forward * realMovementSpeed);
	}
	if (GetAsyncKeyState('D'))
	{
		position = position + (right * realMovementSpeed);
	}
	if (GetAsyncKeyState('A'))
	{
		position = position + (right * -realMovementSpeed);
	}
	if (GetAsyncKeyState('E'))
	{
		position = position + (up * realMovementSpeed);
	}
	if (GetAsyncKeyState('Q'))
	{
		position = position + (up * -realMovementSpeed);
	}

	Vector3 currentRotation = transform.GetEularRotation();
	Vector3 targetRotation = currentRotation;
	Vector2 dPos = InputMapper::GetInstance()->GetMouseDelta();
	if (dPos.x != 0 || dPos.y != 0)
	{
		float realRotationSpeed = 1 * Timer::GetInstance()->GetDeltaTime();

		targetRotation.y += realRotationSpeed * dPos.x;
		targetRotation.x += realRotationSpeed * dPos.y;

		constexpr float maxPitchAngle = 3.14159265359f / 2.0f - 0.01f;
		targetRotation.x = CLAMP(targetRotation.x, -maxPitchAngle, maxPitchAngle);
	}

	float lerpSpeed = 30.0f;
	Vector3 smoothedRotation = {};
	smoothedRotation.x = std::lerp(currentRotation.x, targetRotation.x, lerpSpeed * Timer::GetInstance()->GetDeltaTime());
	smoothedRotation.y = std::lerp(currentRotation.y, targetRotation.y, lerpSpeed * Timer::GetInstance()->GetDeltaTime());
	smoothedRotation.z = 0.0f;
	transform.SetRotation(smoothedRotation);

	transform.SetPosition(position);
	InputMapper::GetInstance()->CaptureMouse();
}

void InitGameWorld()
{
	gameworld.Init();
}

void RenderGameWorld()
{
	gameworld.Render();
}
