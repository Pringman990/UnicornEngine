#pragma once
#include <Core.h>
#include "EditorWindow.h"
#include <Renderer.h>
#include <FreeLookCamera.h>

class SceneWindow : public EditorWindow
{
public:
	SceneWindow(Editor* EditorPtr);
	~SceneWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	Renderer* mRenderer;
	FreeLookCamera* mFLCamera;
	
	//RenderTarget* mSceneView;

	ImTextureID imguiTex;
	bool mIsTextureValid = false;
	class Sampler* sampler;
};

//REGISTER_EDITORWINDOW(SceneWindow)
