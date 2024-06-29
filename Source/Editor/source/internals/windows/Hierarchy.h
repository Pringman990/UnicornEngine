#pragma once
#include "ToolWindow.h"

class Hierarchy : public ToolWindow
{
public:
	Hierarchy(Editor& aEditor);
	~Hierarchy();

	void Init() override;
	void Draw(float aDeltaTime) override;

private:

};