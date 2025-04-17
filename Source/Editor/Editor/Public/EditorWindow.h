#pragma once
#include <Core.h>

class EditorWindow
{
public:
	EditorWindow() {};
	virtual ~EditorWindow() {};

	virtual bool Init() = 0;
	virtual void Render() = 0;

	const std::string& GetDisplayName() const { return mWindowDisplayName; };

protected:
	friend class EditorWindowManager;

	/// <summary>
	/// Can be set in the windows constructor or Init functions.
	/// If no name is set then EditorWindowManager will set a name based on the class name.
	/// </summary>
	std::string mWindowDisplayName = "";

	ImGuiWindowFlags mWindowCreationFlags = 0;

	bool mIsWindowOpen = true;
};