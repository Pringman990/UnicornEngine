#pragma once

class RootSignatureManager
{
public:
	RootSignatureManager();
	~RootSignatureManager();

	ID3D12RootSignature* GetOrCreateRootSignature(const std::string& aShaderByteCode);

private:
	std::unordered_map<std::size_t, ComPtr<ID3D12RootSignature>> mRootSignatureCache;
};