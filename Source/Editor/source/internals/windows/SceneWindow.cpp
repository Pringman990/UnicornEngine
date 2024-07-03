#include "EditorPch.h"
#include "SceneWindow.h"

#include "../EditorCamera.h"
#include <DX11.h>
#include <RenderTarget.h>

SceneWindow::SceneWindow(EditorCore& aEditor) : ToolWindow(aEditor)
{
	mWindowFlags =
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoDecoration;
}

SceneWindow::~SceneWindow()
{
}

void SceneWindow::Init()
{
	mEditorCamera = std::make_shared<EditorCamera>();

	mRendertarget = std::make_shared<RenderTarget>();
	mRendertarget->Create(Vector2(64,64));
}

void SceneWindow::Draw(float aDeltaTime)
{
	{
		mRendertarget->Clear();
		mRendertarget->SetAsActiveRenderTarget();
		Engine::GetGraphicsEngine().SetActiveCamera(mEditorCamera->GetCamera());
		Engine::GetECSSystemManager().RunRenderEngineSystems();
		Engine::GetGraphicsEngine().GetDX11()->GetBackBuffer()->SetAsActiveRenderTarget();

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

std::shared_ptr<RenderTarget> SceneWindow::GetRenderTarget()
{
	return mRendertarget;
}

std::shared_ptr<Camera> SceneWindow::GetCamera()
{
	return mEditorCamera->GetCamera();
}
