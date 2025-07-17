#pragma once
#include <Core.h>
#include "EditorWindow.h"

class ContentWindow : public EditorWindow
{
public:
	ContentWindow(Editor* EditorPtr);
	~ContentWindow() override;

	virtual bool Init() override;
	virtual void Render() override;
private:

};

//REGISTER_EDITORWINDOW(ContentWindow)