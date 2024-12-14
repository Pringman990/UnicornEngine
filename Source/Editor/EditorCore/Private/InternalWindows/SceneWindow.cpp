#include "pch.h"
#include "SceneWindow.h"

#include <Renderer.h>
#include <Timer.h>
#include <Camera.h>

SceneWindow::SceneWindow()
	:
	mSceneView(nullptr)
{
	mWindowDisplayName = "Scene";
	mWindowCreationFlags = (
		ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoScrollbar | 
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse 
		);
}

SceneWindow::~SceneWindow()
{
	mSceneView->Destroy();
}

bool SceneWindow::Init()
{
	RenderTargetResourceManager* manager = ResourceRegistry::GetInstance()->GetManager<RenderTargetResourceManager>();
	mSceneView = manager->CreateRenderTarget(Vector2(128,128));
	
	return true;
}

void SceneWindow::Render()
{
	mEditorCamera.Update(Timer::GetInstance()->GetDeltaTime());

	mSceneView->Clear();

	static ImVec2 lastWindowSize = ImVec2();
	ImVec2 currentWindowSize = ImGui::GetWindowSize();

	if (currentWindowSize.x != lastWindowSize.x || currentWindowSize.y != lastWindowSize.y)
	{
		//TODO: Update camera projection matrix to match the new size.
		//Also rember to always update the camera when changing to another rendertarget so the aspect ratio matches
		mSceneView->Resize(Vector2(currentWindowSize.x, currentWindowSize.y));
		mEditorCamera.GetCamera()->SetPerspective(90, (currentWindowSize.x / currentWindowSize.y), 0.001f, 1000.f);
		lastWindowSize = currentWindowSize;
	}

	mSceneView->Bind();

	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image((ImTextureID)(intptr_t)mSceneView->GetTexture()->GetUnderlyingSRV(), currentWindowSize);
}
