#include "Sampler.h"

Sampler::Sampler()
    :
    mSampler(VK_NULL_HANDLE)
{
}

Sampler::~Sampler()
{
    vkDestroySampler(*Renderer::Get()->GetDevice(), mSampler, nullptr);
}

Sampler* Sampler::Create(
    VkFilter MagFilter, 
    VkFilter MinFilter,
    VkSamplerAddressMode AddressModeU,
    VkSamplerAddressMode AddressModeV, 
    VkSamplerAddressMode AddressModeW, 
    bool AnisotropyEnable,
    float MaxAnisotropy
)
{
    Sampler* sampler = new Sampler();

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = MagFilter;
    samplerInfo.minFilter = MinFilter;
    samplerInfo.addressModeU = AddressModeU;
    samplerInfo.addressModeV = AddressModeV;
    samplerInfo.addressModeW = AddressModeW;
    samplerInfo.anisotropyEnable = AnisotropyEnable ? VK_TRUE : VK_FALSE;
    samplerInfo.maxAnisotropy = AnisotropyEnable ? MaxAnisotropy : 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(*Renderer::Get()->GetDevice(), &samplerInfo, nullptr, &sampler->mSampler))
    {
        _THROW_RENDERER("Failed to create sampler");
        delete sampler;
        return nullptr;
    }
    
    return sampler;
}
