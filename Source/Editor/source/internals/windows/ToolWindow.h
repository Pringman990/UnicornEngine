#pragma once

class ToolWindow
{
public:
	ToolWindow(Editor& aEditor) : mEditor(aEditor) {};
	~ToolWindow() {};

	virtual void Init() {};
	virtual void Draw(float aDeltaTime) = 0;

	bool IsOpen() { return mIsOpen; };
	void SetIsOpen(bool isOpen) { mIsOpen = isOpen; };
	ImGuiWindowFlags GetWindowFlags() { return mWindowFlags; };

protected:
	Editor& mEditor;
	bool mIsOpen = false;
	ImGuiWindowFlags mWindowFlags = 0;
};