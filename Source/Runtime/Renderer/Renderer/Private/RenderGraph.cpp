#include "RenderGraph.h"

#include "RenderPassBuilder.h"

RenderGraph::RenderGraph()
{
}

RenderGraph::~RenderGraph()
{
}

void RenderGraph::AddPass(
	const String& Name, 
	Func<void(RenderPassBuilder&)> BuilderFunc, 
	Func<void(CommandBuffer*)> ExecuteFunc
)
{
	Pass pass;
	pass.name = Name;
	pass.execute = ExecuteFunc;
	
	RenderPassBuilder builder(pass);
	BuilderFunc(builder);

	mPasses.push_back(pass);
}

void RenderGraph::Build()
{
	/*for (uint32 i = 0; i < mPasses.size(); i++)
	{
		auto& passA = mPasses[i];

		for (uint32 j = 0; j < mPasses.size(); j++)
		{
			if (i == j)
				continue;

			auto& passB = mPasses[j];

			bool dependency = false;

			for (auto& write : passA.writes)
			{
				if (std::find(passB.reads.begin(), passB.reads.end(), write) != passB.reads.end())
				{
					dependency = true;
					break;
				}
			}

			if (!dependency)
			{
				for (auto& write : passA.writes)
				{
					if (std::find(passB.reads.begin(), passB.reads.end(), write) != passB.reads.end())
					{
						dependency = true;
						break;
					}
				}
			}

			if (dependency)
			{
				passB.dependencies.push_back(&passA);
			}
		}*/
	//}

	/*Set<const Pass*> visited;

	Func<void(const Pass*)> Visit = [&](const Pass* pass)
		{
			if (visited.contains(pass))
				return;

			visited.insert(pass);

			for (auto* dep : pass->dependencies)
			{
				Visit(dep);
			}

			mExecutionOrder.push_back(pass);
		};

	for (const auto& pass : mPasses)
	{
		Visit(&pass);
	}*/
}

CommandPool* RenderGraph::Execute()
{
	for (auto pass : mExecutionOrder)
	{
		CommandBuffer* cmd = mCommandPool->AllocateCommandBuffer();
		cmd->Begin();
		pass->execute(cmd);
		cmd->End();
	}

	mCommandPool->Reset();
	mPasses.clear();
	mExecutionOrder.clear();

	return mCommandPool;
}
