#pragma once
#include <Core.h>
#include "EditorWindow.h"
//#include <RenderTarget/RenderTarget.h>
#include "EditorCamera.h"


class SceneWindow : public EditorWindow
{
public:
	SceneWindow(Editor* EditorPtr);
	~SceneWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	//RenderTarget* mSceneView;
	EditorCamera mEditorCamera;

	ImTextureID imguiTex;
	class Sampler* sampler;
};

//REGISTER_EDITORWINDOW(SceneWindow)
