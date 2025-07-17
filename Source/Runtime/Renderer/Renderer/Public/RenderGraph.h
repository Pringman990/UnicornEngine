#pragma once
#include <Core.h>

class RenderPassBuilder;

class RenderGraph
{
public:
	struct Pass
	{
		String name;
		Vector<ScopedHandle> reads;
		Vector<ScopedHandle> writes;
		Func<void(CommandBuffer*)> execute;

		Vector<Pass*> dependencies;
	};
public:
	RenderGraph();
	~RenderGraph();

	void AddPass(const String& Name, Func<void(RenderPassBuilder&)> BuilderFunc, Func<void(CommandBuffer*)> ExecuteFunc);

	void Build();
	CommandPool* Execute();

private:
	CommandPool* mCommandPool;
	Vector<Pass> mPasses;
	Vector<const Pass*> mExecutionOrder;
};

#define RG_ADD_PASS(Name, BuilderLambda, ExecuteLambda) \
	Renderer::Get()->GetCurrentFrameSyncRenderGraph().AddPass(Name, \
		BuilderLambda, \
		ExecuteLambda )