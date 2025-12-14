#include "pch.h"
#include "Editor.h"

#include "EditorWindowManager.h"
#include "EditorWindowsIncludes.generated.h"

#include "ImguiBackendFactory.h"

#include <Scene/SceneManager.h>

Editor::Editor()
	:
	mImguiBackend(nullptr)
{
}

Editor::~Editor()
{
	//EditorWindowManager::Shutdown();

	delete mImguiBackend;
	mImguiBackend = nullptr;

	mWindowManager->ClearAllWindows();
}

bool Editor::Init()
{
	//mTextureSampler = Sampler::Create();

	mImguiBackend = ImguiBackendFactory::CreateBackend();
	ENSURE(mImguiBackend, "ImguiBackend was null");

	if (!mImguiBackend->Init())
	{
		LOG_CRITICAL("Imgui backend failed to init");
		return false;
	}

	mWindowManager = SubsystemManager::Get<EditorWindowManager>();
	ENSURE(mWindowManager, "Editor Window Manager was null");

	RegisterEditorWindows();

	return true;
}

void Editor::BeginFrame()
{
	mImguiBackend->BeginFrame();
}

void Editor::Render()
{
	RenderMainMenuBar();

	mWindowManager->RenderActiveWindows();

	mImguiBackend->RenderFrame();
}

void Editor::EndFrame()
{
	mPreviousFrameDrawCalls = 0;

	mImguiBackend->EndFrame();
	/*ImDrawData* drawData = ImGui::GetDrawData();
	int imguiDrawCalls = 0;

	for (int i = 0; i < drawData->CmdListsCount; i++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[i];
		Renderer::Get()->AddToEditorDrawCalls(cmd_list->CmdBuffer.Size);
		mPreviousFrameDrawCalls += Renderer::Get()->GetEditorDrawCalls();
	}*/
}

void Editor::SetSelectedItem(SelectedItemType Type, const SelectedItemVariant& Item)
{
	mSelectedItem.type = Type;
	mSelectedItem.item = Item;
}

void Editor::InvalidateSelectedItem()
{
	mSelectedItem.type = SelectedItemType::Non;
	mSelectedItem.item = std::monostate();
}

void Editor::RenderMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(ICON_FA_FILE "\tFile"))
		{
			if (ImGui::BeginMenu("Load"))
			{
				SceneManager* sceneManager = SubsystemManager::Get<SceneManager>();
				const auto& scenes = sceneManager->GetAllScenes();
				for (auto& [uuid, scene] : scenes)
				{
					if (ImGui::MenuItem(scene->GetName().c_str()))
					{
						Scene* activeScene = sceneManager->GetActiveScene();
						if (scene->GetName() != activeScene->GetName())
						{
							if (!sceneManager->LoadScene(scene->GetName()))
							{
								LOG_ERROR("Failed to load scene");
								break;
							}
							sceneManager->UnloadScene(activeScene->GetName());
						}
						else
						{
							sceneManager->UnloadScene(activeScene->GetName());
							if (!sceneManager->LoadScene(scene->GetName()))
							{
								LOG_ERROR("Failed to load scene");
								break;
							}
						}

						sceneManager->SetActiveScene(scene->GetName());
						break;
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save"))
			{
				SceneManager* sceneManager = SubsystemManager::Get<SceneManager>();
				if (!sceneManager->SaveActiveScene())
				{
					LOG_ERROR("Failed to Save scene");
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void Editor::RegisterEditorWindows()
{
	mWindowManager->RegisterWindowType("SceneWindow", [this]() {return new SceneWindow(this); });
	mWindowManager->CreateWindow("SceneWindow");

	mWindowManager->RegisterWindowType("GraphicsDebugWindow", [this]() {return new GraphicsDebugWindow(this); });
	mWindowManager->CreateWindow("GraphicsDebugWindow");

	mWindowManager->RegisterWindowType("DebugInformationWindow", [this]() {return new DebugInformationWindow(this); });
	mWindowManager->CreateWindow("DebugInformationWindow");

	mWindowManager->RegisterWindowType("ECSDebugWindow", [this]() {return new ECSDebugWindow(this); });
	mWindowManager->CreateWindow("ECSDebugWindow");

	mWindowManager->RegisterWindowType("ModuleWindow", [this]() {return new ModuleWindow(this); });
	mWindowManager->CreateWindow("ModuleWindow");

	mWindowManager->RegisterWindowType("ReflectionRegistryWindow", [this]() {return new ReflectionRegistryWindow(this); });
	mWindowManager->CreateWindow("ReflectionRegistryWindow");

	mWindowManager->RegisterWindowType("SceneHierarchyWindow", [this]() {return new SceneHierarchyWindow(this); });
	mWindowManager->CreateWindow("SceneHierarchyWindow");

	mWindowManager->RegisterWindowType("InspectorWindow", [this]() {return new InspectorWindow(this); });
	mWindowManager->CreateWindow("InspectorWindow");
}
