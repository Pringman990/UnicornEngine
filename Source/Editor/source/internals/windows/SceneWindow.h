#pragma once
#include "ToolWindow.h"

class EditorCamera;
class Camera;
class RenderTarget;

class SceneWindow : public ToolWindow
{
public:
	SceneWindow(EditorCore& aEditor);
	~SceneWindow();

	void Init() override;
	void Draw(float aDeltaTime) override;

	std::shared_ptr<RenderTarget> GetRenderTarget();
	std::shared_ptr<Camera> GetCamera();

private:
	std::shared_ptr<EditorCamera> mEditorCamera;
	std::shared_ptr<RenderTarget> mRendertarget;
};