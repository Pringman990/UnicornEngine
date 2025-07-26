#include "pch.h"
#include "SceneWindow.h"

#include <Renderer.h>
#include <Timer/Timer.h>
//#include <Camera.h>

#include <imgui_impl_vulkan.h>
#include <Sampler.h>
#include <GPUBarrier.h>

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
	delete sampler;
}

bool SceneWindow::Init()
{
	//RenderTargetResourceManager* manager = AssetRegistry::Get()->GetManager<RenderTargetResourceManager>();
	//mSceneView = manager->CreateRenderTarget(Vector2(128,128));
	
	//sampler = Sampler::Create();
	//imguiTex = (ImTextureID)ImGui_ImplVulkan_AddTexture(*sampler, Renderer::Get()->GetOffscreenTexture(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	Renderer* renderer = Renderer::Get();
	GPUTexture* offscreenTexture = renderer->GetGPUResourceManager().GetResource(renderer->GetOffscreenTexture());

	//mEditor->AddTextureToImgui();

	return true;
}

void SceneWindow::Render()
{
	mEditorCamera.Update(Timer::Get()->GetDeltaTime());
//	mSceneView->Clear();

	static ImVec2 lastWindowSize = ImVec2();
	ImVec2 currentWindowSize = ImGui::GetWindowSize();

	if (currentWindowSize.x != lastWindowSize.x || currentWindowSize.y != lastWindowSize.y)
	{
		//TODO: Update camera projection matrix to match the new size.
		//Also rember to always update the camera when changing to another rendertarget so the aspect ratio matches
	//	mSceneView->Resize(Vector2(currentWindowSize.x, currentWindowSize.y));
	//	mEditorCamera.GetCamera()->SetPerspective(90, (currentWindowSize.x / currentWindowSize.y), 0.001f, 1000.f);
	//	lastWindowSize = currentWindowSize;
	}

//	mSceneView->Bind();

	//ImGui::SetCursorPos(ImVec2(0, 0));
	//ImGui::Image(imguiTex, currentWindowSize);
}
