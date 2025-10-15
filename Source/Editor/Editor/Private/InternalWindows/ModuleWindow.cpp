#include "pch.h"
#include "ModuleWindow.h"

#include <Module/ModuleManager.h>

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
