#include "pch.h"
#include "Shader/InputLayoutManager.h"

InputLayoutManager::InputLayoutManager(IInputLayoutFactory* aFactory) : mFactory(aFactory)
{
}

InputLayoutManager::~InputLayoutManager()
{
	for (auto& it : mLayouts)
	{
		delete it.second;
		it.second = nullptr;
	}
	mLayouts.clear();

	delete mFactory;
	mFactory = nullptr;
}

InputLayout* InputLayoutManager::TryGetLayout(eInputLayoutStandardTypes aType, Shader* aVertexShader)
{
	auto it = mLayouts.find(aType);
	if (it != mLayouts.end())
	{
		return it->second;
	}

	InputLayout* layout = nullptr;
	if(aType == eInputLayoutStandardTypes::eModel)
		layout = mFactory->GetModelLayout(aVertexShader);

	if (layout)
		mLayouts.insert({aType, layout });

	return layout;
}
