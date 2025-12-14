#pragma once
#include <EngineMinimal.h>
#include "EditorWindow.h"
#include <EWorld.h>

class InspectorWindow : public EditorWindow
{
public:
	InspectorWindow(Editor* EditorPtr);
	~InspectorWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	class SceneManager* mSceneManager;
};