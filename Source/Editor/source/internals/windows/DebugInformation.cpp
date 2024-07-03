#include "DebugInformation.h"
#include "EditorPch.h"

DebugInformation::DebugInformation(EditorCore& aEditor) : ToolWindow(aEditor)
{
}

DebugInformation::~DebugInformation()
{
}

void DebugInformation::Draw(float /*aDeltaTime*/)
{
	ImGui::Text("FPS: %d", Engine::GetInstance().GetTimer().GetFps());
	ImGui::Text("Draw Calls: %d", Engine::GetGraphicsEngine().GetDrawCalls());
	ImGui::Text("Selected Entities Count: %d", mEditor.GetSelectedEntities().size());
}
