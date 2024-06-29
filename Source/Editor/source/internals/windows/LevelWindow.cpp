#include "EditorPch.h"
#include "LevelWindow.h"

#include "../EditorCamera.h"
#include <source/DX11.h>
#include <source/RenderTarget.h>

LevelWindow::LevelWindow(Editor& aEditor) : ToolWindow(aEditor)
{
	mWindowFlags =
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoDecoration;
}

LevelWindow::~LevelWindow()
{
}

void LevelWindow::Init()
{
	mEditorCamera = std::make_shared<EditorCamera>();
	mRendertarget = std::make_shared<RenderTarget>();
	mRendertarget->Create(Vector2(64,64));
}

void LevelWindow::Draw(float aDeltaTime)
{
	{
		if(ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
			mEditorCamera->Update(aDeltaTime);

		Vector2 size = Vector2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		static Vector2 lastSize;
		if (size != lastSize)
		{
			lastSize = size;
			mRendertarget->Resize(size);
		}

		ImGui::Image(mRendertarget->GetSRV(), ImGui::GetWindowSize());
	}
}

std::shared_ptr<RenderTarget> LevelWindow::GetRenderTarget()
{
	return mRendertarget;
}

std::shared_ptr<Camera> LevelWindow::GetCamera()
{
	return mEditorCamera->GetCamera();
}
