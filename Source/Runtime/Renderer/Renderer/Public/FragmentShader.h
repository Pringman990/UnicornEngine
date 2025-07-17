#pragma once
#include <Core.h>

class FragmentShader
{
public:
	FragmentShader();
	~FragmentShader();

	static FragmentShader* Create(const String& Name, const String& EntryPoint = "main");

	VkPipelineShaderStageCreateInfo GetStageCreateInfo() const;

private:
	VkShaderModule mModule;
	String mEntryPoint;
};