#include "EditorPch.h"
#include "EditorCore.h"

//ToolWindows
#include "windows/ToolWindow.h"
#include "windows/Inspector.h"
#include "windows/DebugInformation.h"
#include "windows/GameWindow.h"
#include "windows/SceneWindow.h"
#include "windows/Hierarchy.h"

#include <source/Window.h>

EditorCore::EditorCore()
{
}

EditorCore::~EditorCore()
{
}

bool EditorCore::Init()
{
	if (!mImguiImpl.Init())
		return false;

	SetStyleEditor();

	RegisterToolWindows();
	
	for (auto& [name, window] : mToolWindows)
	{
		window->Init();
	}

	return true;
}

void EditorCore::Render()
{

}

void EditorCore::RegisterToolWindows()
{
	mToolWindows["Inspector"] = std::make_shared<Inspector>(*this);
	mToolWindows["Hierarchy"] = std::make_shared<Hierarchy>(*this);
	mToolWindows["Game"] = std::make_shared<GameWindow>(*this);
	mToolWindows["Scene"] = std::make_shared<SceneWindow>(*this);
	mToolWindows["DebugInformation"] = std::make_shared<DebugInformation>(*this);
}

void RenderCustomTitleBar(HWND hwnd)
{
	RECT rect;
    GetClientRect(hwnd, &rect);

    // Convert client coordinates to screen coordinates
    POINT topLeft = { rect.left, rect.top };
    ClientToScreen(hwnd, &topLeft);

    ImGui::SetNextWindowPos(ImVec2((float)topLeft.x, (float)topLeft.y - 30.0f));
    ImGui::SetNextWindowSize(ImVec2((float)(rect.right - rect.left), 30.0f));
    ImGui::Begin("TitleBar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    // Custom title bar content
    ImGui::Text("My Custom Title Bar");

    // Implement window dragging
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImGuiIO& io = ImGui::GetIO();
        auto delta = io.MouseDelta;
        RECT windowRect;
        GetWindowRect(hwnd, &windowRect);
        SetWindowPos(hwnd, 0, windowRect.left + (int)delta.x, windowRect.top + (int)delta.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    ImGui::End();
}

void EditorCore::Update(float aDeltaTime)
{
	//RenderCustomTitleBar(Engine::GetGraphicsEngine().GetCurrentWindow().GetWindowInfo().currentWindow);

	for (auto& [name, window] : mToolWindows)
	{
		if (name == "DebugInformation")
			continue;

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
	std::shared_ptr<ToolWindow> debugWindow = mToolWindows["DebugInformation"];
	if (ImGui::Begin("DebugInformation", 0, debugWindow->GetWindowFlags()))
	{
		debugWindow->SetIsOpen(true);
		debugWindow->Draw(aDeltaTime);
	}
	else
	{
		debugWindow->SetIsOpen(false);
	}
	ImGui::End();
}

ImguiImpl& EditorCore::GetImguiImpl()
{
	return mImguiImpl;
}

void EditorCore::AddEntityToSelection(uint32_t anEntity)
{
	mSelectedEntities.push_back(anEntity);
}

void EditorCore::RemoveEntityFromSelection(uint32_t anEntity)
{
	auto it = std::find(mSelectedEntities.begin(), mSelectedEntities.end(), anEntity);
	if(it != mSelectedEntities.end())
		mSelectedEntities.erase(it);
}

void EditorCore::ClearEntitiesFromSelection()
{
	mSelectedEntities.clear();
}

bool EditorCore::IsEntitySelected(ecs::Entity anEntity)
{
	auto it = std::find(mSelectedEntities.begin(), mSelectedEntities.end(), anEntity);
	if (it != mSelectedEntities.end())
		return true;

	return false;
}

const std::vector<uint32_t>& EditorCore::GetSelectedEntities()
{
	return mSelectedEntities;
}

std::shared_ptr<ToolWindow> EditorCore::GetWindow(std::string aWindowName)
{
	return mToolWindows[aWindowName];
}

void EditorCore::SetStyleEditor()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.51f, 0.51f, 0.51f, 0.67f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.02f, 0.60f, 0.06f, 0.84f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.32f, 0.32f, 0.32f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.59f, 0.59f, 0.59f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.67f, 0.67f, 0.67f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.47f, 0.47f, 0.47f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.78f, 0.78f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.20f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.73f, 0.73f, 0.73f, 0.70f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.34f, 0.34f, 0.34f, 0.90f);
}

void EditorCore::SetStylePlayMode()
{
}
