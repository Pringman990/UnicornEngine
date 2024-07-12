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
class SceneSerializer;

class Engine
{
public:
	static Engine& GetInstance();

	static bool Start();
	static void ShutDown();
	
	static GraphicsEngine& GetGraphicsEngine();
	static ecs::SystemManager& GetECSSystemManager();
	static SceneManager& GetSceneManager();
	static SceneSerializer& GetSceneSerializer();

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
	static void ShutdownSceneManager(SceneManager* aSceneManager);
	static void ShutdownSceneSerializer(SceneSerializer* aSceneSerializer);
private:
	static Engine* mInstance;

	std::unique_ptr<GraphicsEngine, void(*)(GraphicsEngine*)> mGraphicsEngine;
	std::unique_ptr<ecs::SystemManager, void(*)(ecs::SystemManager*)> mECSSystemManager;
	std::unique_ptr<SceneManager, void(*)(SceneManager*)> mSceneManager;
	std::unique_ptr<SceneSerializer, void(*)(SceneSerializer*)> mSceneSerializer;

	bool mShouldClose;
	Timer mTimer;
};
