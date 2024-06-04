#include "EditorPch.h"
#include "Editor.h"

#include "EditorCamera.h"

Editor::Editor()
{
}

Editor::~Editor()
{
}

bool Editor::Init()
{
	if (!mImguiImpl.Init())
		return false;

	mEditorCamera = std::make_shared<EditorCamera>();
	return true;
}

void Editor::Update(float aDeltaTime)
{
	mEditorCamera->Update(aDeltaTime);
}

ImguiImpl& Editor::GetImguiImpl()
{
	return mImguiImpl;
}
