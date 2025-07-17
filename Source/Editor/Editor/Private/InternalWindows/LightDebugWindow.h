#pragma once
#include <Core.h>
#include "EditorWindow.h"

class LightDebugWindow : public EditorWindow
{
public:
	LightDebugWindow(Editor* EditorPtr);
	~LightDebugWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:

}; 
//REGISTER_EDITORWINDOW(LightDebugWindow)