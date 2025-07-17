#include "pch.h"
#include "DebugInformationWindow.h"

#include <Timer/Timer.h>
#include <Renderer.h>

DebugInformationWindow::DebugInformationWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
	mWindowDisplayName = "Debug Information";
}

DebugInformationWindow::~DebugInformationWindow()
{
}

bool DebugInformationWindow::Init()
{
	return true;
}

void DebugInformationWindow::Render()
{
	//static bool vsync = Renderer::Get()->IsVsyncEnabled();

	//if (ImGui::Checkbox("Vsync", &vsync))
	//{
	//    Renderer::Get()->SetVsync(vsync);
	//}

	String fps = "FPS: ";
	fps = std::to_string(Timer::Get()->GetFps());
	ImGui::Text(fps.c_str());

	String editorDrawcalls = "Editor DrawCalls: ";
	editorDrawcalls = std::to_string(mEditor->GetPreviousFrameDrawCalls());
	ImGui::Text(editorDrawcalls.c_str());

	String gameDrawcalls = "Game DrawCalls: ";
	gameDrawcalls = std::to_string(Renderer::Get()->GetGameDrawCalls());
	ImGui::Text(gameDrawcalls.c_str());

	//String totalDrawcalls = "Total DrawCalls: ";
	//totalDrawcalls = std::to_string(Renderer::Get()->GetTotalDrawCalls());
	//ImGui::Text(totalDrawcalls.c_str());
}
