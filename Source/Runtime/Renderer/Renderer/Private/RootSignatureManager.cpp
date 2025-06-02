#include "pch.h"
#include "RootSignatureManager.h"

RootSignatureManager::RootSignatureManager()
{
}

RootSignatureManager::~RootSignatureManager()
{
}

ID3D12RootSignature* RootSignatureManager::Create(const D3D12_ROOT_SIGNATURE_DESC& SignatureDesc, const String& Name)
{
	auto it = mRootSignatureCache.find(Name);
	if (it != mRootSignatureCache.end()) 
	{
		_LOG_RENDERER_WARNING("Root signature with name: {}, Already exists.", Name);
		return nullptr;
	}

	ComPtr<ID3DBlob> serializedRootSig;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3D12SerializeRootSignature(&SignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_WARNING("Failed serializing root signature, hr: {}, errorblob: {}", hr, (char*)errorBlob->GetBufferPointer());
		return nullptr;
	}

	ComPtr<ID3D12RootSignature> rootSignature;
	hr = Renderer::GetInstance()->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
	if (FAILED(hr))
	{
		_LOG_RENDERER_WARNING("Failed create root signature, error: {}", hr);
		return nullptr;
	}

	mRootSignatureCache[Name] = rootSignature;

	return rootSignature.Get();
}

ID3D12RootSignature* RootSignatureManager::Get(const String& Name)
{
	auto it = mRootSignatureCache.find(Name);
	if (it == mRootSignatureCache.end())
	{
		_LOG_RENDERER_WARNING("Root signature with name: {}, doesn't exist.", Name);
		return nullptr;
	}
	return it->second.Get();
}
