#include "pch.h"
#include "RootSignatureManager.h"

RootSignatureManager::RootSignatureManager()
{
}

RootSignatureManager::~RootSignatureManager()
{
}

ID3D12RootSignature* RootSignatureManager::GetOrCreateRootSignature(const std::string& aShaderByteCode)
{
	std::size_t hash = std::hash<std::string>{}(aShaderByteCode);

	auto it = mRootSignatureCache.find(hash);
	if (it != mRootSignatureCache.end()) 
	{
		return it->second.Get();
	}

	ComPtr<ID3D12ShaderReflection> shaderReflection;
	D3DReflect(aShaderByteCode.data(), aShaderByteCode.size(), IID_PPV_ARGS(shaderReflection.GetAddressOf()));

	D3D12_SHADER_DESC shaderDesc = {};
	shaderReflection->GetDesc(&shaderDesc);

	D3D12_ROOT_SIGNATURE_DESC signatureDesc = {};
	std::vector<D3D12_ROOT_PARAMETER> rootParameters(shaderDesc.BoundResources);

	for (uint32 i = 0; i < shaderDesc.BoundResources; i++)
	{
		D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
		shaderReflection->GetResourceBindingDesc(i, &bindDesc);

		D3D12_ROOT_PARAMETER param = {};
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_DESCRIPTOR_RANGE range = {};
		range.BaseShaderRegister = bindDesc.BindPoint;
		range.NumDescriptors = 1;
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		
		param.DescriptorTable.NumDescriptorRanges = 1;
		param.DescriptorTable.pDescriptorRanges = &range;

		rootParameters[i] = param;
	}

	signatureDesc.NumParameters = static_cast<uint32>(rootParameters.size());
	signatureDesc.pParameters = rootParameters.data();
	signatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> serializedRootSig;
	ComPtr<ID3DBlob> errorBlob;
	D3D12SerializeRootSignature(&signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	ComPtr<ID3D12RootSignature> rootSignature;
	Renderer::GetInstance()->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));

	mRootSignatureCache[hash] = rootSignature;

	return rootSignature.Get();
}
