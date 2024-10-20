#pragma once
#include "RenderCommands/IRenderCommand.h"

class RenderQueue
{
public:
	RenderQueue();
	~RenderQueue();

	void AddCommand(IRenderCommand* aCommand);
	void Execute();
	void Sort();

private:
	TVector<IRenderCommand*> mCommands;
};