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


//TODO: Remove temporary
enum OFBD_FLAGS { OFBD_FLAGS_OPEN = 0x1, OFBD_FLAGS_SAVE = 0x2 };
int OpenFileBrowserDialog(char* path, const char* filter, const char* extension, OFBD_FLAGS flags)
{
	char working_directory[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(sizeof(working_directory), working_directory);

	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = path;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = extension;

	BOOL result = FALSE;

	switch (flags)
	{
	case OFBD_FLAGS_OPEN:
		result = GetOpenFileNameA(&ofn);
		break;
	case OFBD_FLAGS_SAVE:
		result = GetSaveFileNameA(&ofn);
		break;
	}

	if (result != TRUE)
	{
		SetCurrentDirectoryA(working_directory);
		return 1;
	}

	SetCurrentDirectoryA(working_directory);
	return 0;
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

	mWindowManager = new EditorWindowManager();
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

bool Editor::RenderTextInputBox()
{
	bool result = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 center = viewport->GetCenter();

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Appearing);

	if (ImGui::BeginPopupModal(mInputTextDialogInfo.windowText.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text((mInputTextDialogInfo.inputText + ":").c_str());
		ImGui::SetNextItemWidth(-1);

		if(ImGui::IsWindowAppearing())
			ImGui::SetKeyboardFocusHere();

		ImGui::InputText(
			("##" + mInputTextDialogInfo.inputText).c_str(), 
			mInputTextDialogInfo.input, 
			IM_ARRAYSIZE(mInputTextDialogInfo.input),
			ImGuiInputTextFlags_AutoSelectAll
		);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		bool nameFree = true;
		SceneManager* sceneManager = SceneManager::Instance();
		const auto& scenes = sceneManager->GetAllScenes();
		for (const auto& [uuid, scene] : scenes)
		{
			if (mInputTextDialogInfo.input == scene->GetName())
				nameFree = false;
		}

		bool valid = mInputTextDialogInfo.input[0] != '\0' && nameFree;
		ImGui::BeginDisabled(!valid);
		if (ImGui::Button("Confirm", ImVec2(120,0)))
		{
			mInputTextDialogInfo.confirmed = true;
			result = true;
			ImGui::CloseCurrentPopup();
			mInputTextDialogInfo.open = false;
		}
		ImGui::EndDisabled();

		if (ImGui::IsKeyPressed(ImGuiKey_Enter) && valid)
		{
			mInputTextDialogInfo.confirmed = true;
			result = true;
			ImGui::CloseCurrentPopup();
			mInputTextDialogInfo.open = false;
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)) ||
			ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			mInputTextDialogInfo.confirmed = false;
			result = true;
			ImGui::CloseCurrentPopup();
			mInputTextDialogInfo.open = false;
		}

		ImGui::EndPopup();
	}

	return result;
}

void Editor::OpenNewSceneCreatePopup()
{
	mInputTextDialogInfo.open = true;
	mInputTextDialogInfo.confirmed = false;
	mInputTextDialogInfo.input[0] = '\0';
	mInputTextDialogInfo.windowText = "Create New Scene";
	mInputTextDialogInfo.inputText = "Scene Name";
}

void Editor::RenderMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(ICON_FA_FILE "\tFile"))
		{
			if (ImGui::MenuItem("New"))
			{
				OpenNewSceneCreatePopup();
			}

			if (ImGui::BeginMenu("Load"))
			{
				SceneManager* sceneManager = SceneManager::Instance();
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
				SceneManager* sceneManager = SceneManager::Instance();
				switch (sceneManager->SaveActiveScene().code)
				{
				case SceneErrorCode::ErrorCode::OK:
					break;
				case SceneErrorCode::ErrorCode::InvalidPath:
				{
					//TODO: Open file browser dialog and select path then call sceneManager->SaveActiveSceneToFile();
					FileSystem* fs = FileSystem::Instance();
					
					Path contentPath = fs->GetAbsolutPath("engine://");
					char path[MAX_PATH] = {0};
					
					contentPath = sceneManager->GetActiveScene()->GetName();
					std::snprintf(path, MAX_PATH, "%s", contentPath.c_str());

					if (OpenFileBrowserDialog(path, "Scene Files (*.scene)\0*.scene\0All Files (*.*)\0*.*\0", "scene", OFBD_FLAGS_SAVE))
					{
						LOG_ERROR("Failed to Save scene");
						break;
					}

					if (!fs->Exists(path))
					{
						ByteBuffer buffer;
						fs->WriteAll(path, buffer);
					}

					sceneManager->SaveActiveSceneToFile(path);

					break;
				}
				default:
					LOG_ERROR("Failed to Save scene");
					break;
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (mInputTextDialogInfo.open)
	{
		if (!ImGui::IsPopupOpen(mInputTextDialogInfo.windowText.c_str(), ImGuiPopupFlags_AnyPopup))
			ImGui::OpenPopup(mInputTextDialogInfo.windowText.c_str());

		if (RenderTextInputBox())
		{
			if (mInputTextDialogInfo.confirmed)
			{
				SceneManager* sceneManager = SceneManager::Instance();
				sceneManager->CreateScene(mInputTextDialogInfo.input);
				sceneManager->SetActiveScene(mInputTextDialogInfo.input);

			}

			mInputTextDialogInfo = {};
		}
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
