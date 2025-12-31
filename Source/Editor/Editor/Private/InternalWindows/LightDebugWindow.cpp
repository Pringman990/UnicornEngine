#include "pch.h"
#include "LightDebugWindow.h"

#include <EditorWindowManager.h>
REGISTER_EDITOR_WINDOW(LightDebugWindow, "92b8e2bc-a1bb-49aa-8509-a85b46afad3f");

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
