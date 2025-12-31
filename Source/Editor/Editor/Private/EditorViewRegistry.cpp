#include "EditorViewRegistry.h"

EditorViewRegistry::EditorViewRegistry()
{
}

EditorViewRegistry::~EditorViewRegistry()
{
}

EDITOR_API EditorViewRegistry* EditorViewRegistry::Instance()
{
	static EditorViewRegistry* instance = new EditorViewRegistry();
	return instance;
}

EDITOR_API EditorViewRegistry::EditorViewFunction EditorViewRegistry::GetFunction(UniqueID128 TypeID)
{
	auto it = mFunctions.find(TypeID);
	if (it != mFunctions.end())
		return it->second;

	return nullptr;
}