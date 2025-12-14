#pragma once
#include <Core.h>
#include <EngineMinimal.h>
#include "EditorWindow.h"
#include <EWorld.h>

class ECSDebugWindow : public EditorWindow
{
public:
	ECSDebugWindow(Editor* EditorPtr);
	~ECSDebugWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	EWorld mWorld;
	UnorderedMap<EEntity, bool> mSelectedEntities;
	//UnorderedMap<TypeIdHash, bool> mSelectedComponents;
	EEntity mSelectedEntity;
	UniqueID128 mSelectedComponent = {};
};