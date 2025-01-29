#pragma once

class EditorWindow;

class EditorWindowManager
{
public:
	using WindowCreator = std::function<EditorWindow* ()>;

	static EditorWindowManager* GetInstance()
	{
		static EditorWindowManager instance;
		return &instance;
	}
	void Shutdown() {};

	void RegisterWindowType(const std::string& aType, WindowCreator aCreator) {
		mRegisteredWindows[aType] = std::move(aCreator);
	}

#undef CreateWindow
	void CreateWindow(const std::string& aWindowType);

	void RenderActiveWindows();

private:
	friend class Singleton<EditorWindowManager>;
	EditorWindowManager();
	~EditorWindowManager();
private:
	std::unordered_map<std::string, WindowCreator> mRegisteredWindows;
	std::vector<EditorWindow*> mActiveWindows;
};
