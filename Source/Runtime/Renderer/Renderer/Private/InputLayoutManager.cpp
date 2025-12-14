#include "InputLayoutManager.h"

#include "Renderer.h"
#include "ShaderManager.h"

#include <d3d11.h>
#include <d3dcompiler.h>

InputLayoutManager::InputLayoutManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

InputLayoutManager::~InputLayoutManager()
{
}

InputLayout* InputLayoutManager::TryGetLayout(void* VsBlob, const ShaderReflectionInfo& VsReflection)
{
	ID3DBlob* vsBlob = static_cast<ID3DBlob*>(VsBlob);

	ComPtr<ID3DBlob> signatureBlob;
	HRESULT hr = D3DGetInputSignatureBlob(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		signatureBlob.GetAddressOf()
	);

	if (FAILED(hr))
	{
		LOG_ERROR("Failed to reflect input layout: {}", hr);
		return nullptr;
	}

	size_t hash = HashSignatureBlob(vsBlob);

	auto it = mLayouts.find(hash);
	if (it != mLayouts.end())
	{
		return it->second.get();
	}

	Vector<D3D11_INPUT_ELEMENT_DESC> inputDescs;
	inputDescs.resize(VsReflection.inputParams.size());
	for (uint32 i = 0; i < VsReflection.inputParams.size(); i++)
	{
		inputDescs[i].SemanticName = VsReflection.inputParams[i].semanticName.c_str();
		inputDescs[i].SemanticIndex = VsReflection.inputParams[i].semanticIndex;
		inputDescs[i].Format = ToDXFormat(VsReflection.inputParams[i].format);
		inputDescs[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescs[i].InputSlot = 0;
		inputDescs[i].InstanceDataStepRate = 0;
	}

	OwnedPtr<InputLayout> layout = MakeOwned<InputLayout>();
	hr = mRenderer->GetLogicalDevice()->CreateInputLayout(
		inputDescs.data(),
		static_cast<uint32>(inputDescs.size()),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		layout->layout.GetAddressOf()
	);

	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create input layout: {}", hr);
		return nullptr;
	}

	mLayouts[hash] = std::move(layout);

	return mLayouts[hash].get();
}

size_t InputLayoutManager::HashSignatureBlob(ID3DBlob* Signature)
{
	const uint8_t* data = (const uint8_t*)Signature->GetBufferPointer();
	size_t len = Signature->GetBufferSize();
	size_t hash = 1469598103934665603ull;

	for (size_t i = 0; i < len; ++i)
		hash = (hash ^ data[i]) * 1099511628211ull;

	return hash;
}
