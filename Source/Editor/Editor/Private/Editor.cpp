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
	EditorWindowManager::GetInstance()->Shutdown();

	delete mImguiBackend;
	mImguiBackend = nullptr;
}

bool Editor::Init()
{
	mImguiBackend = ImguiBackendFactory::CreateBackend();
	_ENSURE_EDITOR(mImguiBackend, "ImguiBackend was null");

	if (!mImguiBackend->Init())
	{
		_LOG_EDITOR_CRITICAL("Imgui backend failed to init");
		return false;
	}

	//TODO: remove hardcoding windows
	EditorWindowManager::GetInstance()->CreateWindow("SceneWindow");
	EditorWindowManager::GetInstance()->CreateWindow("GraphicsDebugWindow");
	EditorWindowManager::GetInstance()->CreateWindow("DebugInformationWindow");
	//EditorWindowManager::GetInstance()->CreateWindow("ChunkDebugWindow");

	return true;
}

void Editor::BeginFrame()
{
	mImguiBackend->BeginFrame();
}

void Editor::Render()
{

	
	EditorWindowManager::GetInstance()->RenderActiveWindows();

}

void Editor::EndFrame()
{
	mImguiBackend->EndFrame();

}
