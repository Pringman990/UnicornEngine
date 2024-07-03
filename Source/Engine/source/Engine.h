#pragma once
#include "timer/Timer.h"

namespace ecs
{
	class SystemManager;
}
namespace reflection
{
	class Registry;
}

class GraphicsEngine;
class SceneManager;

class Engine
{
public:
	static Engine& GetInstance();

	static bool Start();
	static void ShutDown();
	
	static GraphicsEngine& GetGraphicsEngine();
	static ecs::SystemManager& GetECSSystemManager();
	static reflection::Registry& GetReflectionRegistry();
	static SceneManager& GetSceneManager();

	void InitEngineSystems();

	bool BeginFrame();
	void Update(bool RunGame);
	void Render(bool RunGame);
	void EndFrame();

	void SetShouldClose(bool shouldClose);

	const Timer& GetTimer() { return mTimer; };

private:
	Engine();
	~Engine();

	bool Init();
	static void ShutdownGrapicsEngine(GraphicsEngine* aGraphicsEngine);
	static void ShutdownSystemManager(ecs::SystemManager* aSystemManager);
	static void ShutdownReflectionRegistry(reflection::Registry* aReflectionRegistry);
	static void ShutdownSceneManager(SceneManager* aSceneManager);
private:
	static Engine* mInstance;

	std::unique_ptr<GraphicsEngine, void(*)(GraphicsEngine*)> mGraphicsEngine;
	std::unique_ptr<ecs::SystemManager, void(*)(ecs::SystemManager*)> mECSSystemManager;
	std::unique_ptr<reflection::Registry, void(*)(reflection::Registry*)> mReflectionRegistry;
	std::unique_ptr<SceneManager, void(*)(SceneManager*)> mSceneManager;

	bool mShouldClose;
	Timer mTimer;
};
