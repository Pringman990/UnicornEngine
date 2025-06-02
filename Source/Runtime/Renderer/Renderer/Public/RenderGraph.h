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
	String name;
	Func<void()> execute;

	Vector<ResourceHandle> reads;
	Vector<ResourceHandle> writes;
};

class RenderGraph
{
public:
	RenderPass& AddPass(const String& DebugName);
	
	const ResourceHandle CreateResource();

	void Compile();
	void Execute();

private:
	friend class Renderer;
	RenderGraph();
	~RenderGraph();

private:
	Vector<RenderPass> mPasses;
};