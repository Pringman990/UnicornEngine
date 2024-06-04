#pragma once
#include "imgui/ImguiImpl.h"

class EditorCamera;

class  Editor
{
public:
	Editor();
	~Editor();

	bool Init();
	void Update(float aDeltaTime);

	ImguiImpl& GetImguiImpl();

private:
	std::shared_ptr<EditorCamera> mEditorCamera;
	ImguiImpl mImguiImpl;
};