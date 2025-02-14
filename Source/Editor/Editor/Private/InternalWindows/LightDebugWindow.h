#pragma once
#include "EditorWindow.h"

class LightDebugWindow : public EditorWindow
{
public:
	LightDebugWindow();
	~LightDebugWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:

}; 
REGISTER_EDITORWINDOW(LightDebugWindow)