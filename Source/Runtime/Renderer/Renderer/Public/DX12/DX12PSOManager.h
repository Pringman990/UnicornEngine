//#pragma once
//#include <Core.h>
//
//struct PipelineStateDesc
//{
//    enum class Type
//    {
//        Undefined,
//        Graphics,
//        Compute
//    } type = Type::Undefined;
//
//	//TODO: Add more data from D3D12_GRAPHICS_PIPELINE_STATE_DESC
//
//	D3D12_SHADER_BYTECODE vs;
//	D3D12_SHADER_BYTECODE ps;
//    D3D12_BLEND_DESC blendState;
//    D3D12_RASTERIZER_DESC rasterizerState;
//    D3D12_DEPTH_STENCIL_DESC depthStencilState;
//    DXGI_FORMAT rtvFormats[8];
//    UINT numRenderTargets;
//    DXGI_FORMAT dsvFormat;
//    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType;
//    DXGI_SAMPLE_DESC sampleDesc;
//    D3D12_INPUT_LAYOUT_DESC inputLayout;
//    String rootSignature;
//};
//
//class PSOManager
//{
//public:
//    ID3D12PipelineState* Create(const PipelineStateDesc& Desc);
//
//private:
//	friend class Renderer;
//	PSOManager();
//	~PSOManager();
//private:
//    ComPtr<ID3D12PipelineState> CreateGraphicsPSO(const PipelineStateDesc& Desc);
//
//    size_t HashPSO(const PipelineStateDesc& Desc);
//private:
//	UnorderedMap<std::size_t, ComPtr<ID3D12PipelineState>> mPipelineStates;
//};