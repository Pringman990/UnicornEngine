#pragma once
#include <Core.h>

struct RenderTargetMetadata
{
	String debugName;
};

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

private:
	RenderTargetMetadata mMetadata;
};