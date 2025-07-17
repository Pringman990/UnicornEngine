#pragma once
#include <Core.h>
#include "EditorWindow.h"

class DebugInformationWindow : public EditorWindow
{
public:
	DebugInformationWindow(Editor* EditorPtr);
	~DebugInformationWindow();

	virtual bool Init() override;
	virtual void Render() override;

private:

};

// REGISTER_EDITORWINDOW(DebugInformationWindow)