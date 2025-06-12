//#include "pch.h"
//#include "PSOManager.h"
//
//#include "RootSignatureManager.h"
//
//PSOManager::PSOManager()
//{
//}
//
//PSOManager::~PSOManager()
//{
//}
//
//ID3D12PipelineState* PSOManager::Create(const PipelineStateDesc& Desc)
//{
//	size_t hash = HashPSO(Desc);
//	auto it = mPipelineStates.find(hash);
//	if (it != mPipelineStates.end())
//	{
//		return it->second.Get();
//	}
//
//	ComPtr<ID3D12PipelineState> pso = nullptr;
//
//	switch (Desc.type)
//	{
//	case PipelineStateDesc::Type::Graphics:
//	{
//		pso = CreateGraphicsPSO(Desc);
//		mPipelineStates.insert({ hash, pso });
//		break;
//	}
//	case PipelineStateDesc::Type::Compute:
//	{
//
//		break;
//	}
//	default:
//		_LOG_RENDERER_ERROR("Trying to create undefined PSO");
//		break;
//	}
//
//	return pso.Get();
//}
//
//ComPtr<ID3D12PipelineState> PSOManager::CreateGraphicsPSO(const PipelineStateDesc& Desc)
//{
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
//	psoDesc.InputLayout = Desc.inputLayout;
//	psoDesc.pRootSignature = Renderer::Get()->GetRootSignatureManager().Get(Desc.rootSignature);
//	psoDesc.VS = Desc.vs;
//	psoDesc.PS = Desc.ps;
//	psoDesc.RasterizerState = Desc.rasterizerState;
//	psoDesc.BlendState = Desc.blendState;
//	psoDesc.DepthStencilState = Desc.depthStencilState;
//	psoDesc.SampleDesc = Desc.sampleDesc;
//	psoDesc.SampleMask = UINT_MAX;
//	psoDesc.PrimitiveTopologyType = Desc.topologyType;
//	psoDesc.NumRenderTargets = Desc.numRenderTargets;
//	memcpy(psoDesc.RTVFormats, Desc.rtvFormats, sizeof(DXGI_FORMAT) * _countof(Desc.rtvFormats));
//
//	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
//	HRESULT hr = Renderer::Get()->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
//	if (FAILED(hr)) 
//	{
//		_LOG_RENDERER_ERROR("Failed to create graphics pso");
//	}
//
//	return pipelineState;
//}
//
//size_t PSOManager::HashPSO(const PipelineStateDesc& Desc)
//{
//	size_t hash = 0;
//
//	hash_combine(hash, HashMemory(Desc.vs.pShaderBytecode, Desc.vs.BytecodeLength));
//	hash_combine(hash, HashMemory(Desc.ps.pShaderBytecode, Desc.ps.BytecodeLength));
//
//	hash_combine(hash, HashMemory(&Desc.depthStencilState, sizeof(Desc.depthStencilState)));
//	hash_combine(hash, HashMemory(&Desc.rasterizerState, sizeof(Desc.rasterizerState)));
//	hash_combine(hash, HashMemory(&Desc.blendState, sizeof(Desc.blendState)));
//
//	for (UINT i = 0; 0 < Desc.inputLayout.NumElements; i++)
//		hash_combine(hash, HashMemory(&Desc.inputLayout.pInputElementDescs[i], sizeof(D3D12_INPUT_ELEMENT_DESC)));
//
//	return hash;
//}