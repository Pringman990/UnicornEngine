#pragma once
#include <Core.h>

struct ResourceHandle
{
	uint32 id = UINT32_MAX;

	operator uint32() const
	{
		return this->id;
	}

	operator bool() const
	{
		return this->id != UINT32_MAX;
	}
};

struct RenderPass
{
	std::string name;
	std::function<void()> execute;

	std::vector<ResourceHandle> reads;
	std::vector<ResourceHandle> writes;
};

class RenderGraph
{
public:
	RenderPass& AddPass(const std::string& DebugName);
	
	const ResourceHandle CreateResource();

	void Compile();
	void Execute();

private:
	friend class Renderer;
	RenderGraph();
	~RenderGraph();

private:
	std::vector<RenderPass> mPasses;
};