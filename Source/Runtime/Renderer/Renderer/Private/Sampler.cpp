#include "Sampler.h"
#include "LogicalDevice.h"

#include <d3d11.h>

OwnedPtr<Sampler> Sampler::Create(LogicalDevice& Device, D3D11_SAMPLER_DESC Desc)
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	OwnedPtr<Sampler> sampler = MakeOwned<Sampler>();

	HRESULT result = Device->CreateSamplerState(&Desc, &sampler->mSamplerState);
	if (FAILED(result))
	{
		sampler.reset();
		return nullptr;
	}

    return std::move(sampler);
}

Sampler::Sampler()
{
}

Sampler::~Sampler()
{
}
