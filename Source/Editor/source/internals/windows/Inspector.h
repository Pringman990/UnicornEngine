#pragma once
#include "ToolWindow.h"

class Inspector : public ToolWindow
{
public:
	Inspector(EditorCore& aEditor);
	~Inspector();

	void Draw(float aDeltaTime) override;

private:
	void EngineCustomDraw(ecs::ComponentType aType, void*& someData);
	void EngineNameDraw();
private:

};