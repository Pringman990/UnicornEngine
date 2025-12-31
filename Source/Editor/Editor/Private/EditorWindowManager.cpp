#include "pch.h"
#include "EditorWindowManager.h"

#include "EditorWindow.h"

REGISTER_ENGINE_SUBSYSTEM(EditorWindowManager)

EditorWindowManager::EditorWindowManager()
{
}

EditorWindowManager::~EditorWindowManager()
{
	for (int32 i = 0; i < mActiveWindows.size(); i++)
	{
		delete mActiveWindows[i];
		mActiveWindows[i] = nullptr;
	}
	mActiveWindows.clear();
	mRegisteredWindows.clear();
}

EDITOR_API EditorWindowManager* EditorWindowManager::Instance()
{
	static EditorWindowManager* instance = new EditorWindowManager();
	return instance;
}

void EditorWindowManager::AddAndInitWindow(UniqueID128 UUID, String Name)
{
	auto it = mRegisteredWindows.find(UUID);
	if (it != mRegisteredWindows.end())
	{
		EditorWindow* window = it->second.get();
		
		if (!AddUniqueToVector(mActiveWindows, window))
		{
			LOG_WARNING("Window already exist as active");
			return;
		}

		if (mStage == Stage::Static)
			mWindowsToInit.push_back(UUID);
		else if (!window->Init())
		{
			LOG_WARNING("Window failed to init");
			EraseItemFromVector(mActiveWindows, window);
			return;
		}

		if (window->mWindowDisplayName == "")
		{
			window->mWindowDisplayName = Name;
		}
	}
}

void EditorWindowManager::UnregisterWindow(UniqueID128 UUID)
{
	if (mRegisteredWindows.contains(UUID))
	{
		mRegisteredWindows.erase(UUID);
	}
}

void EditorWindowManager::RenderActiveWindows()
{
	for (auto& window : mActiveWindows)
	{
		if (!window->mIsWindowOpen)
			continue;

		ImGui::Begin(window->mWindowDisplayName.c_str(), &window->mIsWindowOpen, window->mWindowCreationFlags);
		window->Render();
		ImGui::End();
	}
}

void EditorWindowManager::ClearAllWindows()
{
	mActiveWindows.clear();
	mRegisteredWindows.clear();
	/*for (uint32 i = 0; i < mActiveWindows.size(); i++)
	{
		delete mActiveWindows[i];
	}*/
}

void EditorWindowManager::InitAllRegisteredWindows()
{
	mStage = Stage::Runtime;

	for (auto& window : mWindowsToInit)
	{
		if (!mRegisteredWindows[window]->Init())
		{
			LOG_ERROR("Failed to init editor window {}", window.ToString());
		}
	}

	mWindowsToInit.clear();
}

