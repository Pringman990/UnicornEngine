#include "LauncherPch.h"
#include <windows.h>

#include <source/Engine.h>
#include <GameWorld.h>

#ifdef _DEBUG
#include <source/internals/EditorCore.h>
#endif // _DEBUG

int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	{
		if (!Engine::Start())
			return 0;

		Engine& engine = Engine::GetInstance();

#ifdef _DEBUG
		EditorCore editor;
		editor.Init();
#endif // _DEBUG

		GameWorld game;
		game.Init();

		while (engine.BeginFrame())
		{
#ifdef _DEBUG
			editor.GetImguiImpl().BeginFrame();
			editor.Render();
#endif // _DEBUG

			engine.Update(false);

#ifdef _RELEASE
			engine.Render(false);
#endif // _RELEASE


#ifdef _DEBUG		
			float deltaTime = engine.GetTimer().GetDeltaTime();
			editor.Update(deltaTime);
			editor.GetImguiImpl().EndFrame();
#endif // _DEBUG

			engine.EndFrame();
		}
	}

	Engine::ShutDown();
	return 0;
}