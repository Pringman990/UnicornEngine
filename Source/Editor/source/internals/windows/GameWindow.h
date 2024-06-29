#pragma once
#include "ToolWindow.h"

class RenderTarget;

class GameWindow : public ToolWindow
{
public:
	GameWindow(Editor& aEditor);
	~GameWindow();

	void Init() override;
	void Draw(float aDeltaTime) override;

	std::shared_ptr<RenderTarget> GetRenderTarget();

private:
	std::shared_ptr<RenderTarget> mRendertarget;
};