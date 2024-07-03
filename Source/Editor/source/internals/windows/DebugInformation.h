#pragma once
#include "ToolWindow.h"

class DebugInformation : public ToolWindow
{
public:
	DebugInformation(EditorCore& aEditor);
	~DebugInformation();

	void Draw(float aDeltaTime) override;

private:

};