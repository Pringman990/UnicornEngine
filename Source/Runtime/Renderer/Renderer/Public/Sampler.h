#pragma once
#include <EngineMinimal.h>
#include <RendererMinimal.h>

struct ID3D11SamplerState;
struct D3D11_SAMPLER_DESC;

class LogicalDevice;

class Sampler
{
public:
	Sampler();
	~Sampler();

	static OwnedPtr<Sampler> Create(LogicalDevice& Device, D3D11_SAMPLER_DESC Desc);

public:
	ID3D11SamplerState* operator->() const noexcept
	{
		return mSamplerState.Get();
	}


private:


private:
	ComPtr<ID3D11SamplerState> mSamplerState;
};