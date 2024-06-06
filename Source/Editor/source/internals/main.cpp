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

		while (engine.BeginFrame())
		{
			editor.GetImguiImpl().BeginFrame();

			float deltaTime = engine.GetTimer().GetDeltaTime();
			editor.Update(deltaTime);

			engine.Update();

			//Set rendertarget to backbuffer again
			engine.Render();

			editor.GetImguiImpl().EndFrame();
			engine.EndFrame();
		}
	}

	GraphicsEngine::Shutdown();
	Engine::ShutDown();
	return 0;
}

