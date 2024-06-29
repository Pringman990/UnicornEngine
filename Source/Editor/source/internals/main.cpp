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
#include <source/Camera.h>
#include "source/ecs/components/Transform.h"
#include "source/ecs/components/Mesh.h"
#include "source/ecs/components/Sprite.h"
#include "windows/GameWindow.h"
#include "windows/LevelWindow.h"
#include "windows/ToolWindow.h"
#include <source/Window.h>

#include <source/sprite/SpriteInstance.h>

extern ecs::World gECSWorld;

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

		ecs::Entity entity = gECSWorld.CreateEntity();
		ecs::Entity entity2 = gECSWorld.CreateEntity();
		
		ecs::Sprite* sprite = gECSWorld.AddComponent(entity, ecs::Sprite());
		sprite->sprite.Init(L"../Assets/Test.dds");
		
		ecs::Transform* transform = gECSWorld.AddComponent(entity, ecs::Transform());
		transform->scale = Vector3(sprite->sprite.GetTexture()->GetTextureSize(), 1);
		transform->position = Vector3(600.f, 360.f, 1.f);

		gECSWorld.AddComponent(entity2, ecs::Transform());
		ecs::Mesh* mesh = gECSWorld.AddComponent(entity2, ecs::Mesh());
		mesh->meshPath = L"../EngineAssets/Models/sm_cube.fbx";
		mesh->modelInstance = graphicsEngine.GetModelFactory().GetModelInstance(mesh->meshPath);

		GameWindow* gameWindow = dynamic_cast<GameWindow*>(editor.GetWindow("Game").get());
		LevelWindow* levelWindow = dynamic_cast<LevelWindow*>(editor.GetWindow("Level").get());

		std::shared_ptr<Camera> tempCamera = std::make_shared<Camera>();
		Vector2 windowSize = graphicsEngine.GetCurrentWindow().GetWindowInfo().resolution;
		tempCamera->SetOrthographic(windowSize, 0.01f, 1000.f);
		tempCamera->GetTransform().SetPosition({0,0,-3});

		while (engine.BeginFrame())
		{
			editor.GetImguiImpl().BeginFrame();

			graphicsEngine.SetActiveCamera(tempCamera);

			if (gameWindow->IsOpen())
			{
				gameWindow->GetRenderTarget()->Clear();
				gameWindow->GetRenderTarget()->SetAsActiveRenderTarget();
				engine.Render();
			}
			if (levelWindow->IsOpen())
			{
				levelWindow->GetRenderTarget()->Clear();
				graphicsEngine.SetActiveCamera(levelWindow->GetCamera());
				levelWindow->GetRenderTarget()->SetAsActiveRenderTarget();

				engine.Render();

				graphicsEngine.SetActiveCamera(tempCamera);
			}
	
			engine.Update();

			graphicsEngine.GetDX11()->GetBackBuffer()->SetAsActiveRenderTarget();

			float deltaTime = engine.GetTimer().GetDeltaTime();
			editor.Update(deltaTime);

			editor.GetImguiImpl().EndFrame();
			
			engine.EndFrame();
		}
	}
	
	reflection::Registry::GetInstance().ShutDown();
	GraphicsEngine::Shutdown();
	Engine::ShutDown();
	return 0;
}

