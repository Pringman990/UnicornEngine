#pragma once
#include "ToolWindow.h"

class EditorCamera;
class Camera;
class RenderTarget;

class LevelWindow : public ToolWindow
{
public:
	LevelWindow(Editor& aEditor);
	~LevelWindow();

	void Init() override;
	void Draw(float aDeltaTime) override;

	std::shared_ptr<RenderTarget> GetRenderTarget();
	std::shared_ptr<Camera> GetCamera();

private:
	std::shared_ptr<EditorCamera> mEditorCamera;
	std::shared_ptr<RenderTarget> mRendertarget;
};