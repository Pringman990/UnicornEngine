#pragma once
#include <Core.h>

class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();

	static ComputeShader* Create(const String& Name, const String& EntryPoint = "main");

	VkPipelineShaderStageCreateInfo GetStageCreateInfo() const;

	void Dispatch(CommandBuffer* Buffer, uint32 GroupCountX, uint32 GroupCountY, uint32 GroupCountZ);

private:
	VkShaderModule mModule;
	String mEntryPoint;
};