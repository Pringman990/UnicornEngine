#pragma once
#include <EngineMinimal.h>
#include <RendererMinimal.h>

#include "CommandList.h"
#include "RenderPass.h"

class RenderGraphBuilder
{
public:
	RenderGraphBuilder() {};
	~RenderGraphBuilder() {};

	void Write() {};
	void Read() {};

private:

};


class RenderGraph
{
public:
	RenderGraph();
	~RenderGraph();

	void AddPass(String Name, void(*)(RenderGraphBuilder& builder), void(*)(CommandList& cmd));

private:
	Vector<RenderPass> mPasses;
};