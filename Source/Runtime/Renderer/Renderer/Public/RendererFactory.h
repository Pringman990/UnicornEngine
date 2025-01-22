#pragma once
#include <Renderer.h>
#include <SimpleMath.h>

class RendererFactory
{
public:
	static Renderer* CreateRenderer();

private:
	RendererFactory() { };
	~RendererFactory() {};
};