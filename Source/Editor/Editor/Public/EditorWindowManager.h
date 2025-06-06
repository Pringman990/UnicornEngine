#pragma once
#include <Core.h>
#include <EngineSubsystem.h>

class EditorWindow;

class EditorWindowManager : public EngineSubsystem<EditorWindowManager>
{
	friend class EngineSubsystem<EditorWindowManager>;
public:
	using WindowCreator = Func<EditorWindow* ()>;

	void RegisterWindowType(const String& Type, WindowCreator Creator) {
		mRegisteredWindows[Type] = std::move(Creator);
	}

#undef CreateWindow
	void CreateWindow(const String& WindowType);

	void RenderActiveWindows();

	EditorWindowManager();
	~EditorWindowManager() override;
private:
private:
	UnorderedMap<String, WindowCreator> mRegisteredWindows;
	Vector<EditorWindow*> mActiveWindows;
};
