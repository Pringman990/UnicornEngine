#include "pch.h"
#include "SceneWindow.h"

#include <Timer/Timer.h>
#include <SwapChain.h>
#include <GPUResources/GPUTextureManager.h>
//#include <Camera.h>
#include <Scene/SceneManager.h>

#include <EditorWindowManager.h>
REGISTER_EDITOR_WINDOW(SceneWindow, "92b8e2bc-a1bb-49aa-8509-a85b46afad3c");

SceneWindow::SceneWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
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
	delete mFLCamera;
	mFLCamera = nullptr;
}

bool SceneWindow::Init()
{
	mRenderer = Renderer::Instance();
	ASSERT(mRenderer, "Renderer was null in the editor scene window Init()");

	auto appInfo = Application::Instance()->GetApplication()->GetWindowInfo();

	mFLCamera = new FreeLookCamera();
	//mFLCamera.SetPerspective(60.f, (16.f/9.f), 0.01f, 1000.f);
	mFLCamera->SetOrthographic({ (int32)appInfo.viewportWidth, (int32)appInfo.viewportHeight }, 0.001f, 1000);
	mFLCamera->SetControlSchema(FreeLookCameraControlSchema::OrthoPan);
	mFLCamera->GetTransform().SetPosition(Vector3(0, 0, -3));
	mRenderer->SetActiveCamera(mFLCamera);

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

	static ImVec2 lastWindowSize = ImVec2();
	ImVec2 currentWindowSize = ImGui::GetWindowSize();

	bool resize = currentWindowSize.x != lastWindowSize.x || currentWindowSize.y != lastWindowSize.y;
	if (resize)
	{
		//TODO: Update camera projection matrix to match the new size.
		//Also rember to always update the camera when changing to another rendertarget so the aspect ratio matches
		mRenderer->ResizeMainRenderTarget(Vector2i(currentWindowSize.x, currentWindowSize.y));
		mFLCamera->HandleResizeEvent(static_cast<int32>(currentWindowSize.x), static_cast<int32>(currentWindowSize.y));
		lastWindowSize = currentWindowSize;
		gpuTex = mRenderer->GetGPUTextureManager()->GetInternalTexture(mRenderer->GetMainRenderTarget().texture);
	}

	ImGui::SetCursorPos(ImVec2(0, 0));
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	ImGui::Image(gpuTex->srv.Get(), currentWindowSize);

	if (!resize && ImGui::IsItemHovered())
		mFLCamera->HandleInputsAndMove();

	//The Image must be above this if statement or the IsItemHovered won't work correctly
	if (!resize && ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		ImVec2 mouse = ImGui::GetMousePos();

		float localX = mouse.x - imagePos.x;
		float localY = mouse.y - imagePos.y;

		uint32 texX = (uint32)(localX);
		uint32 texY = (uint32)(localY);

		texX = CLAMP(texX, 0u, currentWindowSize.x - 1);
		texY = CLAMP(texY, 0u, currentWindowSize.y - 1);

		uint32 pickedID = mRenderer->GetRenderIDFromPosition({ (int32)texX, (int32)texY });
		if (pickedID != 0)
		{
			EEntity pickedEntity = SceneManager::Instance()->GetActiveScene()->GetSceneView().ResolveEntity(pickedID);
			mEditor->SetSelectedItem(SelectedItemType::Entity, pickedEntity);
		}
		else
		{
			mEditor->InvalidateSelectedItem();
		}
	}
}
