#pragma once

class IImguiBackend;

class Editor
{
public:
	Editor();
	~Editor();

	bool Init();
	void BeginFrame();
	void Render();
	void EndFrame();

private:
	void RegisterEditorWindows();
private:
	IImguiBackend* mImguiBackend;
};