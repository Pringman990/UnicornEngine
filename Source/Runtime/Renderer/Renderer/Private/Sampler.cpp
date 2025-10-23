#include "Sampler.h"
#include "LogicalDevice.h"

#include <d3d11.h>

OwnedPtr<Sampler> Sampler::Create(LogicalDevice& Device, D3D11_SAMPLER_DESC Desc)
{
	OwnedPtr<Sampler> sampler = MakeOwned<Sampler>();

	HRESULT result = Device->CreateSamplerState(&Desc, sampler->mSamplerState.GetAddressOf());
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
