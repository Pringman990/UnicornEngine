#pragma once
#include <Core.h>
#include "EditorWindow.h"
#include <Renderer.h>
#include <FreeLookCamera.h>
#include <ImGuizmo.h>

class SceneWindow : public EditorWindow
{
public:
	SceneWindow(Editor* EditorPtr);
	~SceneWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	void RenderGuizmo();
private:
	Renderer* mRenderer;
	SharedPtr<FreeLookCamera> mFLCamera;
	
	//RenderTarget* mSceneView;

	ImTextureID imguiTex;
	bool mIsTextureValid = false;
	class Sampler* sampler;

	ImGuizmo::OPERATION mGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE mGizmoMode = ImGuizmo::MODE::WORLD;

	UnorderedMap<uint32, EEntity> mRenderIDToEntity;
};

//REGISTER_EDITORWINDOW(SceneWindow)
