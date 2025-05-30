#pragma once
#include <Core.h>

#ifdef _WIN64
#include <Windows.h>
#endif

class GenericApplication;
class Renderer;
class FileWatcher;

#ifdef _EDITOR
class Editor;
#endif // _EDITOR

typedef void*(*SandboxRender)();
typedef void*(*SandboxInit)();

class EngineLoop
{
public:
	EngineLoop();
	~EngineLoop();

	bool Init();
	void Update();

	void RequestExit();
	bool EngineLoopClose() const;
private:
	GenericApplication* mGenericApplication;
	Renderer* mRenderer;
	FileWatcher* mFileWatcher;

#ifdef _EDITOR
	Editor* mEditor = nullptr;
#endif // _EDITOR

	SandboxRender mSandboxRender;

	bool mShouldExit;
};