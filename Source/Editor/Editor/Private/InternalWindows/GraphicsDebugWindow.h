#pragma once
#include "EditorWindow.h"

#include <Renderer.h>

class GraphicsDebugWindow : public EditorWindow
{
public:
	GraphicsDebugWindow();
	~GraphicsDebugWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	GraphicsCardInfo mGraphicsCardInfo;
};

REGISTER_EDITORWINDOW(GraphicsDebugWindow)