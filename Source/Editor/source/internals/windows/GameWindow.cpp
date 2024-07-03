#include "EditorPch.h"
#include "GameWindow.h"

#include <RenderTarget.h>

GameWindow::GameWindow(EditorCore& aEditor) : ToolWindow(aEditor)
{
	mWindowFlags =
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoDecoration;
}

GameWindow::~GameWindow()
{
}

void GameWindow::Init()
{
    mRendertarget = std::make_shared<RenderTarget>();
    mRendertarget->Create(Vector2(64, 64));
}

void GameWindow::Draw(float /*aDeltaTime*/)
{
	//mRendertarget->SetAsActiveRenderTarget();
	//Engine::GetGraphicsEngine().SetActiveCamera(nullptr);
	//Engine::GetECSSystemManager().RunRenderEngineSystems();
	//Engine::GetGraphicsEngine().GetDX11()->GetBackBuffer()->SetAsActiveRenderTarget();

	Vector2 size = Vector2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	static Vector2 lastSize;
	if (size != lastSize)
	{
		lastSize = size;
		mRendertarget->Resize(size);
	}

	ImGui::Image(mRendertarget->GetSRV(), ImGui::GetWindowSize());
}

std::shared_ptr<RenderTarget> GameWindow::GetRenderTarget()
{
    return mRendertarget;
}
