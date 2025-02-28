#include "pch.h"
#include "ChunkDebugWindow.h"

#include <ChunkGenerator.h>

ChunkDebugWindow::ChunkDebugWindow()
	:
	ChunkCount(1)
{
	mWindowDisplayName = "Chunk Debug";
}

ChunkDebugWindow::~ChunkDebugWindow()
{
}

bool ChunkDebugWindow::Init()
{
	return true;
}

void ChunkDebugWindow::Render()
{
}
