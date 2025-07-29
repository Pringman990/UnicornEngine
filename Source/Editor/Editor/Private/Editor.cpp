#include "pch.h"
#include "Editor.h"

#include "EditorWindowManager.h"
#include "EditorWindowsIncludes.generated.h"

#include "ImguiBackendFactory.h"
#include <CommandBuffer.h>
#include <Texture2D.h>
#include <Sampler.h>

Editor::Editor()
	:
	mImguiBackend(nullptr)
{
}

Editor::~Editor()
{
	EditorWindowManager::Shutdown();

	delete mImguiBackend;
	mImguiBackend = nullptr;
}

bool Editor::Init()
{
	_PAUSE_TRACK_MEMORY(true); // We turn off tracking because there is a phantom memory leak
	EditorWindowManager::Create();
	_PAUSE_TRACK_MEMORY(false);

	mTextureSampler = Sampler::Create();

	mImguiBackend = ImguiBackendFactory::CreateBackend();
	_ENSURE_EDITOR(mImguiBackend, "ImguiBackend was null");

	if (!mImguiBackend->Init())
	{
		_LOG_EDITOR_CRITICAL("Imgui backend failed to init");
		return false;
	}

	RegisterEditorWindows();

	return true;
}

void Editor::BeginFrame()
{
	Vector<int32> texturesIndexToRemove;
	for (uint32 i = 0; i < mTextureUploadQueue.size(); i++)
	{
		Texture2D* texture = mTextureUploadQueue[i];
		if (!texture->GetResourceData().gpuHandle)
			continue;

		mImguiBackend->AddTextureToImgui(texture, mTextureSampler);
		texturesIndexToRemove.push_back(i);
	}

	for (int32 i = (int32)texturesIndexToRemove.size() - 1; i >= 0; --i)
	{
		size_t index = texturesIndexToRemove[i];
		if (index < mTextureUploadQueue.size()) // sanity check
			mTextureUploadQueue.erase(mTextureUploadQueue.begin() + index);
	}

	mImguiBackend->BeginFrame();
}

void Editor::Render()
{
	EditorWindowManager::Get()->RenderActiveWindows();

	mImguiBackend->RenderFrame();
}

void Editor::EndFrame(CommandBuffer* Buffer)
{
	mPreviousFrameDrawCalls = 0;

	mImguiBackend->EndFrame(Buffer);
	ImDrawData* drawData = ImGui::GetDrawData();
	int imguiDrawCalls = 0;

	for (int i = 0; i < drawData->CmdListsCount; i++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[i];
		Renderer::Get()->AddToEditorDrawCalls(cmd_list->CmdBuffer.Size);
		mPreviousFrameDrawCalls += Renderer::Get()->GetEditorDrawCalls();
	}	
}

void Editor::AddTextureToImgui(Texture2D* Texture)
{
	//mImguiBackend->AddTextureToImgui(Texture, mTextureSampler);
	mTextureUploadQueue.push_back(Texture);
}

void Editor::RegisterEditorWindows()
{
	EditorWindowManager::Get()->RegisterWindowType("SceneWindow", [this]() {return new SceneWindow(this); });
	EditorWindowManager::Get()->CreateWindow("SceneWindow");

	EditorWindowManager::Get()->RegisterWindowType("GraphicsDebugWindow", [this]() {return new GraphicsDebugWindow(this); });
	EditorWindowManager::Get()->CreateWindow("GraphicsDebugWindow");

	EditorWindowManager::Get()->RegisterWindowType("DebugInformationWindow", [this]() {return new DebugInformationWindow(this); });
	EditorWindowManager::Get()->CreateWindow("DebugInformationWindow");
}
