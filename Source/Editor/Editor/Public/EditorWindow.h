#pragma once
#include <Core.h>

#include "Editor.h"
#include <imgui.h>
class EditorWindow
{
public:
	EditorWindow(Editor* EditorPtr) : mEditor(EditorPtr) {};
	virtual ~EditorWindow() {};

	virtual bool Init() = 0;
	virtual void Render() = 0;

	const String& GetDisplayName() const { return mWindowDisplayName; };

protected:
	friend class EditorWindowManager;

	/*
	* Can be set in the windows constructor or Init functions.
	* If no name is set then EditorWindowManager will set a name based on the class name.
	*/
	String mWindowDisplayName = "";

	ImGuiWindowFlags mWindowCreationFlags = 0;

	bool mIsWindowOpen = true;

	Editor* mEditor;
};