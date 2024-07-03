#pragma once
#include "ToolWindow.h"

class AssetsWindow : public ToolWindow
{
public:
	AssetsWindow(EditorCore& aEditor);
	~AssetsWindow();

	void Init() override;
	void Draw(float aDeltaTime) override;

private:

};