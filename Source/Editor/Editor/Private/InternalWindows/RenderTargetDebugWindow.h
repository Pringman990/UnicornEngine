#pragma once
#include <Core.h>
#include "EditorWindow.h"

class RenderTargetDebugWindow : public EditorWindow
{
public:
	RenderTargetDebugWindow(Editor* EditorPtr);
	~RenderTargetDebugWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
};