#pragma once
#include <Core.h>

class IImguiBackend;
class EditorWindowManager;

class Editor
{
public:
	Editor();
	~Editor();

	bool Init();
	void BeginFrame();
	void Render();
	void EndFrame();

	uint32 GetPreviousFrameDrawCalls() const { return mPreviousFrameDrawCalls; };

private:
	void RegisterEditorWindows();
private:
	IImguiBackend* mImguiBackend;
	EditorWindowManager* mWindowManager;
	uint32 mPreviousFrameDrawCalls = 0;
};