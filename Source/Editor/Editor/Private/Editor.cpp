#include "pch.h"
#include "Editor.h"

#include "EditorWindowManager.h"
#include "EditorWindowsIncludes.generated.h"

#include "ImguiBackendFactory.h"

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
	//Vector<int32> texturesIndexToRemove;
	//for (uint32 i = 0; i < mTextureUploadQueue.size(); i++)
	//{
	//	Texture2D* texture = mTextureUploadQueue[i];
	//	if (!texture->GetResourceData().gpuHandle)
	//		continue;

	//	mImguiBackend->AddTextureToImgui(texture, mTextureSampler);
	//	texturesIndexToRemove.push_back(i);
	//}

	//for (int32 i = (int32)texturesIndexToRemove.size() - 1; i >= 0; --i)
	//{
	//	size_t index = texturesIndexToRemove[i];
	//	if (index < mTextureUploadQueue.size()) // sanity check
	//		mTextureUploadQueue.erase(mTextureUploadQueue.begin() + index);
	//}

	mImguiBackend->BeginFrame();
}

void Editor::Render()
{
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
}
