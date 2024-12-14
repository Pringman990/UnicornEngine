#include "pch.h"
#include "Shader/MaterialShader.h"

MaterialShader::MaterialShader()
	:
	Shader(eShaderTypes::eMatrial),
	mVertexShader(nullptr),
	mPixelShader(nullptr)
{

}

MaterialShader::~MaterialShader()
{
}

bool MaterialShader::Bind()
{
	if (!mPixelShader || !mVertexShader)
		return false;

	return (mVertexShader->Bind() && mPixelShader->Bind());
}

void MaterialShader::SetVertexShader(Shader* aVertexShader)
{
	mVertexShader = aVertexShader;
}

void MaterialShader::SetPixelShader(Shader* aPixelShader)
{
	mPixelShader = aPixelShader;
}

Shader* MaterialShader::GetVertexShader() const
{
	return mVertexShader;
}

Shader* MaterialShader::GetPixelShader() const
{
	return mPixelShader;
}
