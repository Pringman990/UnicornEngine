#include "pch.h"
#include "ContentWindow.h"

#include <EditorWindowManager.h>
REGISTER_EDITOR_WINDOW(ContentWindow, "92b8e2bd-a1bb-49aa-8509-a85b46afad3c");

ContentWindow::ContentWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
}

ContentWindow::~ContentWindow()
{
}

bool ContentWindow::Init()
{
	return true;
}

void ContentWindow::Render()
{
}
