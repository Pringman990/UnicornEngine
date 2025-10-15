#pragma once
#include <EngineMinimal.h>

class EditorWindow;

/*
* Engine subsystem
*/
class EditorWindowManager
{
	friend struct subsystem::SubsystemDescriptor;
public:
	using WindowCreator = Func<EditorWindow* ()>;

	void RegisterWindowType(const String& Type, WindowCreator Creator) {
		mRegisteredWindows[Type] = std::move(Creator);
	}

#undef CreateWindow
	void CreateWindow(const String& WindowType);

	void RenderActiveWindows();

	EditorWindowManager();
	~EditorWindowManager();
private:
private:
	UnorderedMap<String, WindowCreator> mRegisteredWindows;
	Vector<EditorWindow*> mActiveWindows;
};
