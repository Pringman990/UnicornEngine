#pragma once
#include <Core.h>
#include <Singleton.h>

class EditorWindow;

class EditorWindowManager : public Singleton<EditorWindowManager>
{
	friend class Singleton<EditorWindowManager>;
public:
	using WindowCreator = Func<EditorWindow* ()>;

	void RegisterWindowType(const String& aType, WindowCreator aCreator) {
		mRegisteredWindows[aType] = std::move(aCreator);
	}

#undef CreateWindow
	void CreateWindow(const String& aWindowType);

	void RenderActiveWindows();

	EditorWindowManager();
	~EditorWindowManager() override;
private:
private:
	UnorderedMap<String, WindowCreator> mRegisteredWindows;
	Vector<EditorWindow*> mActiveWindows;
};
