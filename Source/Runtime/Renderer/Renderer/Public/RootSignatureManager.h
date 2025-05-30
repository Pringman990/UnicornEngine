#pragma once
#include <Core.h>

class RootSignatureManager
{
public:
	RootSignatureManager();
	~RootSignatureManager();

	ID3D12RootSignature* Create(const D3D12_ROOT_SIGNATURE_DESC& SignatureDesc, const std::string& Name);
	ID3D12RootSignature* Get(const std::string& Name);

private:
	std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> mRootSignatureCache;
};