#pragma once
#include "RendererDefines.h"
#include <EngineMinimal.h>

class RENDERER_API RenderLoop
{
public:
	RenderLoop() = default;
	~RenderLoop() = default;

	bool Init();
	void BeginFrame();
	void Execute();
	void EndFrame();
private:
	//Not owning
	class Renderer* mRenderer;
};