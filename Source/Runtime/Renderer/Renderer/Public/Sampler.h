#pragma once
#include <Core.h>

class Sampler
{
public:
	Sampler();
	~Sampler();

    static Sampler* Create(
        VkFilter MagFilter = VK_FILTER_LINEAR,
        VkFilter MinFilter = VK_FILTER_LINEAR,
        VkSamplerAddressMode AddressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        VkSamplerAddressMode AddressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        VkSamplerAddressMode AddressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        bool AnisotropyEnable = false,
        float MaxAnisotropy = 1.0f
    );

    VkSampler GetRaw() const { return mSampler; };

public:
    operator VkSampler() const noexcept { return mSampler; }
private:
	VkSampler mSampler;

};