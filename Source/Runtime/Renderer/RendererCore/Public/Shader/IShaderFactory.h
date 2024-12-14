#pragma once

#include "Shader.h"

class IShaderFactory
{
public:
	IShaderFactory() {};
	virtual ~IShaderFactory() {};

	virtual Shader* CreateVertexShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel) = 0;
	virtual Shader* CreatePixelShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel) = 0;

private:

};