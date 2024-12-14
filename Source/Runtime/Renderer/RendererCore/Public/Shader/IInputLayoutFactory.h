#pragma once
#include "InputLayout.h"
#include "Shader/Shader.h"

class IInputLayoutFactory
{
public:
	IInputLayoutFactory() {};
	virtual ~IInputLayoutFactory() {};

	virtual InputLayout* GetModelLayout(Shader* aVertexShader) = 0;

private:

};