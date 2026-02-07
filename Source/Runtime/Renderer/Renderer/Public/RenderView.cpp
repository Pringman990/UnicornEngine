#include "RenderView.h"

RenderView::RenderView()
{
}

RenderView::~RenderView()
{
}

void RenderView::AddPass(String Name, RenderPass::PassExecuteFn ExecuteFn)
{
	RenderPass pass;
	pass.name = Name;
	pass.execute = ExecuteFn;
	mPasses.push_back(std::move(pass));
}
