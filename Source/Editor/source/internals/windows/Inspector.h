#pragma once
#include "ToolWindow.h"

class Inspector : public ToolWindow
{
public:
	Inspector(Editor& aEditor);
	~Inspector();

	void Draw(float aDeltaTime) override;

private:

};