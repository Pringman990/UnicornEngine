#pragma once
#include "ToolWindow.h"

class AssetsWindow : public ToolWindow
{
public:
	AssetsWindow(Editor& aEditor);
	~AssetsWindow();

	void Init() override;
	void Draw(float aDeltaTime) override;

private:

};