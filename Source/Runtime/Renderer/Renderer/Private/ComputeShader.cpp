#include "ComputeShader.h"

ComputeShader::ComputeShader()
    :
    mModule(VK_NULL_HANDLE)
{
}

ComputeShader::~ComputeShader()
{
    vkDestroyShaderModule(*Renderer::Get()->GetDevice(), mModule, nullptr);
}

ComputeShader* ComputeShader::Create(const String& Name, const String& EntryPoint)
{
    ComputeShader* shader = new ComputeShader();
    shader->mEntryPoint = EntryPoint;

    ByteBuffer buffer = FileSystem::ReadAll("shader://" + Name);

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

VkPipelineShaderStageCreateInfo ComputeShader::GetStageCreateInfo() const
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    createInfo.module = mModule;
    createInfo.pName = mEntryPoint.c_str();
    createInfo.pSpecializationInfo = nullptr; //Good if there are constant varibles in hlsl if statements so the compiler can optimize away them

    return createInfo;
}

void ComputeShader::Dispatch(CommandBuffer* Buffer, uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ)
{
    vkCmdDispatch(*Buffer, GroupCountX, GroupCountY, GroupCountZ);
}
