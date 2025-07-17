#include "pch.h"
#include "LightDebugWindow.h"

LightDebugWindow::LightDebugWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
	mWindowDisplayName = "Lights Debug";
}

LightDebugWindow::~LightDebugWindow()
{
}

bool LightDebugWindow::Init()
{
	return true;
}

void LightDebugWindow::Render()
{

}
