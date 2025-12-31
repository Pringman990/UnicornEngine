#include "pch.h"
#include "ModuleWindow.h"

#include <Module/ModuleManager.h>

#include <EditorWindowManager.h>
REGISTER_EDITOR_WINDOW(ModuleWindow, "92b8e2bc-a1bb-49fa-8509-a85b46afad3c");

ModuleWindow::ModuleWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
	mWindowDisplayName = "Module";
}

ModuleWindow::~ModuleWindow()
{
	
}

bool ModuleWindow::Init()
{
	return true;
}

void ModuleWindow::Render()
{
	/*if (ImGui::Button("Unload Game Module"))
	{
		ModuleManager::Get()->UnLoadModule("Sandbox");
	}

	if (ImGui::Button("Load Game Module"))
	{
		ModuleManager::Get()->LoadModule("Sandbox");
	}*/
}
