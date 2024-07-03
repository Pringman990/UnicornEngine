#pragma once
#include "ToolWindow.h"

class Hierarchy : public ToolWindow
{
public:
	Hierarchy(EditorCore& aEditor);
	~Hierarchy();

	void Init() override;
	void Draw(float aDeltaTime) override;

private:
	void TreeNode(ecs::Entity anEntity);
	void ObjectList();
private:
};