#pragma once
#include <Core.h>

#ifdef _WIN64
#include <Windows.h>
#endif

#include <RenderLoop.h>

class GenericApplication;
class WindowsFileWatcher;

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
	RenderLoop mRenderLoop;
	WindowsFileWatcher* mFileWatcher;

	SandboxRender mSandboxRender;

	bool mShouldExit;
};