#pragma once
#include "EditorWindow.h"

class DebugInformationWindow : public EditorWindow
{
public:
	DebugInformationWindow();
	~DebugInformationWindow();

	virtual bool Init() override;
	virtual void Render() override;

private:

};

// REGISTER_EDITORWINDOW(DebugInformationWindow)