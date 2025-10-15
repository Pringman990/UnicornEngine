#pragma once
#include <Core.h>
#include "EditorWindow.h"

class ModuleWindow : public EditorWindow
{
public:
	ModuleWindow(Editor* EditorPtr);
	~ModuleWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:

};

