#pragma once
#include <EngineMinimal.h>

class RenderLoop
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