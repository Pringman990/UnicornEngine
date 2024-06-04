#pragma once
#include "timer/Timer.h"

class GraphicsEngine;

class Engine
{
public:
	static Engine& GetEngine();

	static bool Start();
	static void ShutDown();

	bool BeginFrame();
	void Update();
	void Render();
	void EndFrame();

	void SetShouldClose(bool shouldClose);

	const Timer& GetTimer() { return mTimer; };

private:
	Engine();
	~Engine();

	bool Init();

private:
	static Engine* mInstance;

	bool mShouldClose;
	Timer mTimer;

};
