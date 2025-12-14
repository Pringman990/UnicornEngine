#include "pch.h"
#include "SceneWindow.h"

#include <Timer/Timer.h>
#include <SwapChain.h>
#include <GPUResources/GPUTextureManager.h>
//#include <Camera.h>

SceneWindow::SceneWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
	//mSceneView(nullptr)
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
	//mSceneView->Destroy();
}

bool SceneWindow::Init()
{
	mRenderer = SubsystemManager::Get<Renderer>();
	ASSERT(mRenderer, "Renderer was null in the editor scene window Init()");

	mFLCamera.SetPerspective(60.f, (16.f/9.f), 0.01f, 1000.f);
	mFLCamera.GetTransform().SetPosition(Vector3(0,0,-3));
	mRenderer->SetActiveCamera(&mFLCamera);

	return true;
}

void SceneWindow::Render()
{
	GPUTexture* gpuTex = mRenderer->GetGPUTextureManager()->GetInternalTexture(mRenderer->GetMainRenderTarget().texture);
	if (!gpuTex)
	{
		LOG_ERROR("Backbuffer not valid in the scene window");
		return;
	}

	if(ImGui::IsWindowHovered())
		mFLCamera.HandleInputsAndMove();

	static ImVec2 lastWindowSize = ImVec2();
	ImVec2 currentWindowSize = ImGui::GetWindowSize();

	if (currentWindowSize.x != lastWindowSize.x || currentWindowSize.y != lastWindowSize.y)
	{
		//TODO: Update camera projection matrix to match the new size.
		//Also rember to always update the camera when changing to another rendertarget so the aspect ratio matches
		mRenderer->ResizeMainRenderTarget(Vector2i(currentWindowSize.x, currentWindowSize.y));
		mFLCamera.HandleResizeEvent(static_cast<int32>(currentWindowSize.x), static_cast<int32>(currentWindowSize.y));
		lastWindowSize = currentWindowSize;
	}

	
	GenericApplication* app = SubsystemManager::Get<Application>()->GetApplication();	
	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image(gpuTex->srv.Get(), currentWindowSize);
}
