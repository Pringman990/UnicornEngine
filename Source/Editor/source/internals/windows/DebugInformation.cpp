#include "DebugInformation.h"
#include "EditorPch.h"

DebugInformation::DebugInformation(Editor& aEditor) : ToolWindow(aEditor)
{
}

DebugInformation::~DebugInformation()
{
}

void DebugInformation::Draw(float /*aDeltaTime*/)
{
	ImGui::Text("FPS: %d", Engine::GetEngine().GetTimer().GetFps());
	ImGui::Text("Draw Calls: %d", GraphicsEngine::GetInstance().GetDrawCalls());
}
