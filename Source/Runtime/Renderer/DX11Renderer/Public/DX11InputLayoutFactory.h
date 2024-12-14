#pragma once
#include <Shader/IInputLayoutFactory.h>



class DX11InputLayoutFactory : public IInputLayoutFactory
{
public:
	DX11InputLayoutFactory();
	~DX11InputLayoutFactory() override;

	virtual InputLayout* GetModelLayout(Shader* aVertexShader) override;

private:
};