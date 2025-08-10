#pragma once
#include <Core.h>

class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();

	static ComputeShader* Create(const String& Name, const String& EntryPoint = "main");

	VkPipelineShaderStageCreateInfo GetStageCreateInfo() const;

private:
	VkShaderModule mModule;
	String mEntryPoint;
};