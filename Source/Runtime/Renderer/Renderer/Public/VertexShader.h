#pragma once
#include <Core.h>

class VertexShader
{
public:
	VertexShader();
	~VertexShader();

	static VertexShader* Create(const String& Name, const String& EntryPoint = "main");

	VkPipelineShaderStageCreateInfo GetStageCreateInfo() const;

private:
	VkShaderModule mModule;
	String mEntryPoint;
};