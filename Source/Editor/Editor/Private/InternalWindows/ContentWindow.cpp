#include "pch.h"
#include "ContentWindow.h"

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
	return false;
}

void ContentWindow::Render()
{
}
