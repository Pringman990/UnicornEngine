#pragma once
#include "RenderPass.h"

#include "IRenderStateManager.h"

class RenderQueue
{
	struct SwapBuffer
	{
		std::vector<RenderPass> mRenderPasses;
	};

public:
	RenderQueue();
	~RenderQueue();

	bool Init();

	void SubmitRenderPass(const RenderPass& aRenderPass);
	//void SwapBuffers();
	void Execute();

	void _SetStateManager(IRenderStateManager* aStateManager);

private:
	void ExecuteRenderPass(const RenderPass& aRenderPass);
	void ExecuteCommandList(const RenderCommandList& aCommandList);
private:
	std::vector<RenderPass> mRenderPasses;

	IRenderStateManager* mRenderStateManager; //TODO: initilize in renderfactory
};