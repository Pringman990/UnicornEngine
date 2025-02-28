#pragma once
#include "EditorWindow.h"

class ChunkDebugWindow : public EditorWindow
{
public:
	ChunkDebugWindow();
	~ChunkDebugWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	int32 ChunkCount;
};

REGISTER_EDITORWINDOW(ChunkDebugWindow)