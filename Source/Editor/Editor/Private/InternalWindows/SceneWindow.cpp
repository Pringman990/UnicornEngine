#include "pch.h"
#include "SceneWindow.h"

#include <Timer/Timer.h>
#include <Components/ETransform.h>
#include <SwapChain.h>
#include <Renderer.h>
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
}

bool SceneWindow::Init()
{
	mRenderer = Renderer::Instance();
	ASSERT(mRenderer, "Renderer was null in the editor scene window Init()");

	auto appInfo = Application::Instance()->GetApplication()->GetWindowInfo();

	mFLCamera = MakeShared<FreeLookCamera>();
	mFLCamera->SetPerspective(60.f, (16.f/9.f), 0.01f, 1000.f);
	mFLCamera->SetControlSchema(FreeLookCameraControlSchema::FreeLook);
	//mFLCamera->SetOrthographic({ (int32)appInfo.viewportWidth, (int32)appInfo.viewportHeight }, 0.001f, 1000);
	//mFLCamera->SetControlSchema(FreeLookCameraControlSchema::OrthoPan);
	mFLCamera->GetTransform().SetPosition(Vector3(0, 0, -3));
	mRenderer->SetActiveCamera(mFLCamera.get());

	return true;
}

void SceneWindow::Render()
{
	mRenderIDToEntity.clear();

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

	RenderView view;
	view.SetCamera(mFLCamera);
	view.AddPass("Opaque", [](RenderPassContext& ctx)
		{
			Renderer* renderer = Renderer::Instance();
			
			ctx.cmd.ClearRenderTarget(renderer->GetMainRenderTarget().texture, Color(0.0f, 0.0f, 0.0f, 1.f));
			ctx.cmd.ClearDepthStencil(renderer->GetMainRenderTarget().dsv);
			ctx.cmd.SetRenderTargets({ renderer->GetMainRenderTarget().texture }, renderer->GetMainRenderTarget().dsv);
			ctx.cmd.SetViewport(renderer->GetMainRenderTarget().texture);

			ctx.cmd.SetSamplers({ renderer->GetSampler()}, 0);
			for (uint32 i = 0; i < ctx.scene.size(); i++)
			{
				ctx.cmd.DrawMesh(ctx.scene[i].mesh.Get(), ctx.scene[i].transform);
			}
		});
	view.AddPass("ObjectIDPicking", [&](RenderPassContext& ctx)
		{
			Renderer* renderer = Renderer::Instance();

			ctx.cmd.ClearRenderTarget(renderer->GetObjectIDTexture(), Color(0.0f, 0.0f, 0.0f, 1.f));
			ctx.cmd.ClearRenderTarget(renderer->GetObjectIDVisualTexture(), Color(0.0f, 0.0f, 0.0f, 1.f));
			
			ctx.cmd.SetRenderTargets({ renderer->GetObjectIDTexture(), renderer->GetObjectIDVisualTexture() }, renderer->GetMainRenderTarget().dsv);
			ctx.cmd.SetViewport(renderer->GetMainRenderTarget().texture);

			ctx.cmd.SetSamplers({ renderer->GetSampler() }, 0);
			for (uint32 i = 0; i < ctx.scene.size(); i++)
			{
				//TODO pushconstant to shader. entity id.
				ctx.cmd.DrawMesh(ctx.scene[i].mesh.Get(), ctx.scene[i].transform);
			}
		});

	auto& renderScene = SceneManager::Instance()->GetActiveScene()->GetRenderScene();
	mRenderer->SubmitRenderView(view, renderScene);

	ImGui::SetCursorPos(ImVec2(0, 0));
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	ImGui::Image(gpuTex->srv.Get(), currentWindowSize);

	if (mEditor->GetPlayState() != EditorPlayState::Play && !resize && ImGui::IsItemHovered())
		mFLCamera->HandleInputsAndMove();

	//The Image must be above this if statement or the IsItemHovered won't work correctly
	/*if (mEditor->GetPlayState() != EditorPlayState::Play && !resize && ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
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
	}*/

	if (mEditor->GetPlayState() != EditorPlayState::Play)
		RenderGuizmo();
}

void SceneWindow::RenderGuizmo()
{
	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mGizmoOperation = ImGuizmo::OPERATION::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		mGizmoOperation = ImGuizmo::OPERATION::SCALE;

	const SelectedItem& item = mEditor->GetSelectedItem();
	if (item.type != SelectedItemType::Entity)
		return;

	SceneManager* sceneManager = SceneManager::Instance();
	EWorld& world = sceneManager->GetActiveScene()->GetWorld();
	EEntity entity = std::get<EEntity>(mEditor->GetSelectedItem().item);
	ETransform* transform = world.GetComponent<ETransform>(entity);

	Matrix matrix;
	matrix *= Matrix::CreateScale(transform->scale);
	matrix *= Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(transform->rotation.GetRadian()));
	matrix *= Matrix::CreateTranslation(transform->position);

	Camera* camera = Renderer::Instance()->GetActiveCamera();
	Matrix view = camera->GetViewMatrix();
	Matrix proj = camera->GetProjectionMatrix();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetOrthographic(false);
	float windowW = (float)ImGui::GetWindowWidth();
	float windowH = (float)ImGui::GetWindowHeight();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowW, windowH);
	ImGuizmo::Manipulate(&view.m[0][0], &proj.m[0][0], mGizmoOperation, mGizmoMode, &matrix.m[0][0]);

	Vector3 scale, translation;
	Quaternion qRotation;
	matrix.Decompose(scale, qRotation, translation);
	transform->position = translation;
	transform->rotation = qRotation.ToEuler().GetDegree();
	transform->scale = scale;
}
