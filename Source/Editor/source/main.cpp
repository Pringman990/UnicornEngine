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

		PointLight pLight({}, Color(0.3f,0.5f,1,1), 1.0f, 2.0f);
		pLight.GetTransform().SetPosition(Vector3(0,0,2));

		while (engine.BeginFrame())
		{
			editor.GetImguiImpl().BeginFrame();

			float deltaTime = engine.GetTimer().GetDeltaTime();
			editor.Update(deltaTime);

			engine.Update();
			
			ImGui::Begin("Cube");

			Vector3 position = mdl.GetModel()->GetTransform().GetPosition();
			float pos[3] = {position.x, position.y, position.z};
			if (ImGui::DragFloat3("Position", pos, 0.01f))
			{
				position.x = pos[0];
				position.y = pos[1];
				position.z = pos[2];
				mdl.GetModel()->GetTransform().SetPosition(position);
			}

			ImGui::End();

			ImGui::Begin("Scene");
			ImGui::Image(mdl.GetModel()->GetMeshData()[0].material->GetTexture(eMaterialTexture::eColor)->GetSRV(), { 64, 64 });
			ImGui::End();

			mdl.Render();
			
			graphicsEngine.GetLightManager().ClearPointLights();
			graphicsEngine.GetLightManager().AddPointLight(pLight);
			
			engine.Render();

			editor.GetImguiImpl().EndFrame();
			engine.EndFrame();
		}
	}

	GraphicsEngine::Shutdown();
	Engine::ShutDown();
	return 0;
}

