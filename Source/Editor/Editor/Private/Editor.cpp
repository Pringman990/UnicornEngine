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
	EditorWindowManager::Shutdown();

	delete mImguiBackend;
	mImguiBackend = nullptr;
}

bool Editor::Init()
{
	_PAUSE_TRACK_MEMORY(true); // We turn off tracking because there is a phantom memory leak
	EditorWindowManager::Create();
	_PAUSE_TRACK_MEMORY(false);

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
	mImguiBackend->BeginFrame();
}

void Editor::Render()
{
	EditorWindowManager::Get()->RenderActiveWindows();

	mImguiBackend->RenderFrame();
}

void Editor::EndFrame()
{
	mImguiBackend->EndFrame();
}

void Editor::RegisterEditorWindows()
{
	EditorWindowManager::Get()->RegisterWindowType("SceneWindow", []() {return new SceneWindow(); });
	EditorWindowManager::Get()->CreateWindow("SceneWindow");

	EditorWindowManager::Get()->RegisterWindowType("GraphicsDebugWindow", []() {return new GraphicsDebugWindow(); });
	EditorWindowManager::Get()->CreateWindow("GraphicsDebugWindow");

	EditorWindowManager::Get()->RegisterWindowType("DebugInformationWindow", []() {return new DebugInformationWindow(); });
	EditorWindowManager::Get()->CreateWindow("DebugInformationWindow");
}
