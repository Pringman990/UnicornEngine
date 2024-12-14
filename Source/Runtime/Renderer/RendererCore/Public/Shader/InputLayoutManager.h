#pragma once
#include "IInputLayoutFactory.h"
#include "InputLayout.h"
#include "Shader/Shader.h"

class InputLayoutManager
{
public:
	InputLayoutManager(IInputLayoutFactory* aFactory);
	~InputLayoutManager();

	InputLayout* TryGetLayout(eInputLayoutStandardTypes aType, Shader* aVertexShader);

private:
	IInputLayoutFactory* mFactory;
	std::unordered_map<eInputLayoutStandardTypes, InputLayout*> mLayouts;
};