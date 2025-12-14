#pragma once
#include <EngineMinimal.h>
#include "EditorWindow.h"

#include <EWorld.h>

class SceneHierarchyWindow : public EditorWindow
{
public:
	SceneHierarchyWindow(Editor* EditorPtr);
	~SceneHierarchyWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	void DrawEntityNode(const EEntity& Entity, EWorld& World);
private:
	class SceneManager* mSceneManager;
	EEntity mSelectedEntity;
	EEntity mRenamingEntity;
	char mRenameBuffer[256];
};