#include "Systems/StaticMeshRenderSystem.h"

#include <Core.h>
#include <ESystemManager.h>

#include <Timer/Timer.h>
#include <Renderer.h>

#include "Components/ETransform.h"
#include "Components/EStaticMesh.h"

#include "Scene/SceneManager.h"

void StaticMeshRenderSystem(EWorld& World)
{
#ifdef _DEBUG
	Timer::Instance()->StartReading("StaticMeshRenderSystem");
#endif // _DEBUG

	auto query = World.Query<ETransform, EStaticMesh>();

	Scene* scene = SceneManager::Instance()->GetActiveScene();

	for (uint32 i = 0; i < query.GetCount(); i++)
	{
		ETransform& transform = query.Get<ETransform>(i);
		EStaticMesh& mesh = query.Get<EStaticMesh>(i);

		if (!mesh.mesh)
		{
			LOG_WARNING("Trying to render a EStaticMesh with a invalid mesh asset");
			continue;
		}

		Transform trans;
		trans.SetPosition(transform.position);
		trans.SetRotation(transform.rotation);
		trans.SetScale(transform.scale);
		
		scene->GetSceneView().SubmitMesh(query.GetEntity(i), mesh.mesh, trans);
		//Renderer::Instance()->SubmitMesh(mesh.mesh, trans);
	}

#ifdef _DEBUG
	float systemTime = Timer::Instance()->EndReading("StaticMeshRenderSystem");

	ESystemDebugInfo debugInfo{};
	debugInfo.entityCount = query.GetCount();
	debugInfo.frameRunTime = systemTime;
	ESystemManager::Instance()->RegisterFrameDebugInfo("StaticMeshRenderSystem", debugInfo);
#endif // _DEBUG
}

REGISTER_ESYSTEM(
	STRINGIFYEXP(StaticMeshRenderSystem),
	"fad60f23-00bc-4b84-bdaa-35d87bd93279",
	EPipeline::ESystemPostUpdate,
	EP_Rendering,
	StaticMeshRenderSystem
);
