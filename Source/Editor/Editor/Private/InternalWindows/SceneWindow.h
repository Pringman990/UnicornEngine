#pragma once
#include "EditorWindow.h"
//#include <RenderTarget/RenderTarget.h>
#include "EditorCamera.h"

class SceneWindow : public EditorWindow
{
public:
	SceneWindow();
	~SceneWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	//RenderTarget* mSceneView;
	EditorCamera mEditorCamera;
};

//REGISTER_EDITORWINDOW(SceneWindow)
