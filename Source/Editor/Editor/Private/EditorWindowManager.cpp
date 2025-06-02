#include "pch.h"
#include "EditorWindowManager.h"

#include "EditorWindow.h"

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

void EditorWindowManager::CreateWindow(const String& aWindowType)
{
	auto it = mRegisteredWindows.find(aWindowType);
	if (it != mRegisteredWindows.end())
	{
		auto& windowCallback = it->second;
		auto window = windowCallback();
		
		if (!AddUniqueToVector(mActiveWindows, window))
		{
			_LOG_EDITOR_WARNING("Window already exist as active");
			return;
		}

		if (!window->Init())
		{
			_LOG_EDITOR_WARNING("Window failed to init");
			EraseItemFromVector(mActiveWindows, window);
			return;
		}

		if (window->mWindowDisplayName == "")
		{
			window->mWindowDisplayName = aWindowType;
		}
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

