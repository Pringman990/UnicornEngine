#include "FragmentShader.h"

FragmentShader::FragmentShader() 
    :
    mModule(VK_NULL_HANDLE)
{
}

FragmentShader::~FragmentShader()
{
    vkDestroyShaderModule(*Renderer::Get()->GetDevice(), mModule, nullptr);
}

FragmentShader* FragmentShader::Create(const String& Name, const String& EntryPoint)
{
    FragmentShader* shader = new FragmentShader();
    shader->mEntryPoint = EntryPoint;

    ByteBuffer buffer = FileSystem::Get()->ReadAll("shader://" + Name);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    if (vkCreateShaderModule(*Renderer::Get()->GetDevice(), &createInfo, nullptr, &shader->mModule) != VK_SUCCESS)
    {
        _LOG_RENDERER_ERROR("Failed to create shader module for: {}", Name);
        return nullptr;
    }

    return shader;
}

VkPipelineShaderStageCreateInfo FragmentShader::GetStageCreateInfo() const
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    createInfo.module = mModule;
    createInfo.pName = mEntryPoint.c_str();
    createInfo.pSpecializationInfo = nullptr; //Good if there are constant varibles in hlsl if statements so the compiler can optimize away them

    return createInfo;
}
