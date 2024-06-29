#include "EditorPch.h"
#include "Editor.h"

//ToolWindows
#include "windows/ToolWindow.h"
#include "windows/Inspector.h"
#include "windows/DebugInformation.h"
#include "windows/GameWindow.h"
#include "windows/LevelWindow.h"

//Utility


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

	RegisterToolWindows();
	
	for (auto& [name, window] : mToolWindows)
	{
		window->Init();
	}

	return true;
}

void Editor::RegisterToolWindows()
{
	mToolWindows["Inspector"] = std::make_shared<Inspector>(*this);
	mToolWindows["Game"] = std::make_shared<GameWindow>(*this);
	mToolWindows["Level"] = std::make_shared<LevelWindow>(*this);
	mToolWindows["DebugInformation"] = std::make_shared<DebugInformation>(*this);
}

void Editor::Update(float aDeltaTime)
{
	for (auto& [name, window] : mToolWindows)
	{
		if (ImGui::Begin(name.c_str(), 0, window->GetWindowFlags()))
		{
			window->SetIsOpen(true);
			window->Draw(aDeltaTime);
		}
		else
		{
			window->SetIsOpen(false);
		}
		ImGui::End();
	}
}

ImguiImpl& Editor::GetImguiImpl()
{
	return mImguiImpl;
}

void Editor::AddEntityToSelection(uint32_t anEntity)
{
	mSelectedEntities.push_back(anEntity);
}

void Editor::RemoveEntityFromSelection(uint32_t anEntity)
{
	auto it = std::find(mSelectedEntities.begin(), mSelectedEntities.end(), anEntity);
	if(it != mSelectedEntities.end())
		mSelectedEntities.erase(it);
}

void Editor::ClearEntitiesFromSelection()
{
	mSelectedEntities.clear();
}

const std::vector<uint32_t>& Editor::GetSelectedEntities()
{
	return mSelectedEntities;
}

std::shared_ptr<ToolWindow> Editor::GetWindow(std::string aWindowName)
{
	return mToolWindows[aWindowName];
}
