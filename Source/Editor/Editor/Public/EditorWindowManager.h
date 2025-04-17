#pragma once
#include <Core.h>
#include <Singleton.h>

class EditorWindow;

class EditorWindowManager : public Singleton<EditorWindowManager>
{
	friend class Singleton<EditorWindowManager>;
public:
	using WindowCreator = std::function<EditorWindow* ()>;

	void RegisterWindowType(const std::string& aType, WindowCreator aCreator) {
		mRegisteredWindows[aType] = std::move(aCreator);
	}

#undef CreateWindow
	void CreateWindow(const std::string& aWindowType);

	void RenderActiveWindows();

	EditorWindowManager();
	~EditorWindowManager() override;
private:
private:
	std::unordered_map<std::string, WindowCreator> mRegisteredWindows;
	std::vector<EditorWindow*> mActiveWindows;
};
