#include "pch.h"
#include "RenderQueue.h"

RenderQueue::RenderQueue()
{
}

RenderQueue::~RenderQueue()
{
	mCommands.Clear();
}

void RenderQueue::AddCommand(IRenderCommand* aCommand)
{
	aCommand;
}

void RenderQueue::Execute()
{
}

void RenderQueue::Sort()
{
}
