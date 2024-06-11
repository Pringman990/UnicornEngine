#include "EditorPch.h"

#include <windows.h>
#include <source/Engine.h>
#include <source/GraphicsEngine.h>
#include <source/DX11.h>

#include <source/light/LightManager.h>
#include <source/model/ModelFactory.h>
#include <source/model/ModelInstance.h>
#include <source/model/Model.h>
#include <source/Material.h>
#include <source/RenderTarget.h>
#include <source/ecs/ECSWorld.h>


void ExampleSystem(ecs::World& /*world*/, ecs::Entity entity, ecs::Transform& transform, ecs::Mesh& mesh)
{
	transform;
	mesh.vector.push_back(11);
	ecs::Entity ent = entity;
	ent;
}

int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	//We have this {} because thing need to go out of scope to check for memory leaks, or else std::ptr will be detected as memory leaks
	{
		if (!GraphicsEngine::Start())
			return 0;

		if (!Engine::Start())
			return 0;

		Editor editor;
		if (!editor.Init())
			return 0;

		Engine& engine = Engine::GetEngine();
		GraphicsEngine& graphicsEngine = GraphicsEngine::GetInstance();

		ModelInstance mdl = graphicsEngine.GetModelFactory().GetModelInstance(L"../EngineAssets/Models/sm_cube.fbx");

		//ECS
		auto wrappedExampleSystem = [](ecs::World& world, ecs::Entity entity, ecs::Transform& transform, ecs::Mesh& mesh) {
			ExampleSystem(world, entity, transform, mesh);
			};

		ecs::World world;
		world.BindSystem<ecs::Transform, ecs::Mesh>(wrappedExampleSystem);

		ecs::Entity entity = world.CreateEntity();
		world.AddComponent(entity, ecs::Transform());
		ecs::Mesh mesh;
		mesh.b = 10000;
		mesh.vector.push_back(10);
		mesh.vector.push_back(10);
		mesh.vector.push_back(10);
		mesh.vector.push_back(10);
		world.AddComponent(entity, mesh);
		

		while (engine.BeginFrame())
		{
			editor.GetImguiImpl().BeginFrame();

			float deltaTime = engine.GetTimer().GetDeltaTime();
			editor.Update(deltaTime);

			world.ProcessSystems();

			engine.Update();

			mdl.Render();

			editor.GetImguiImpl().EndFrame();
			engine.EndFrame();
		}
	}

	GraphicsEngine::Shutdown();
	Engine::ShutDown();
	return 0;
}

