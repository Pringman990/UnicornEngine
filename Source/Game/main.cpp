#include "GamePch.h"
#include <windows.h>

#include <source/Engine.h>

//#include <source/GraphicsEngine.h>
//#include <source/light/LightManager.h>

int APIENTRY wWinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	//We have this {} because thing need to go out of scope to check for memory leaks, or else std::ptr will be detected as memory leaks
	//{
	//	if (!Engine::Start())
	//		return 0;

		Engine& engine = Engine::GetEngine();
		engine;
	//	PointLight pLight({}, Color(0.3f, 0.5f, 1, 1), 1.0f, 20.0f);
	//	pLight.GetTransform().SetPosition(Vector3(1, 0, 2));

	//	while (engine.BeginFrame())
	//	{
	//		engine.Update();

	//		engine.GetGraphicsEngine().GetLightManager().ClearPointLights();
	//		engine.GetGraphicsEngine().GetLightManager().AddPointLight(pLight);

	//		engine.Render();

	//		engine.EndFrame();
	//	}
	//}

	//Engine::ShutDown();
	return 0;
}