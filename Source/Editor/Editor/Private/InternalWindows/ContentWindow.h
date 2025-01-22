#pragma once
#include "EditorWindow.h"

class ContentWindow : public EditorWindow
{
public:
	ContentWindow();
	~ContentWindow() override;

	virtual bool Init() override;
	virtual void Render() override;
private:

};

REGISTER_EDITORWINDOW(ContentWindow)