#include "Systems/StaticMeshRenderSystem.h"

#include <Core.h>
#include <ESystemManager.h>


#include <Timer/Timer.h>
#include <Renderer.h>

#include "Components/ETransform.h"
#include "Components/EStaticMesh.h"

void StaticMeshRenderSystem(EWorld& World)
{
#ifdef _DEBUG
	GET_TIMER()->StartReading("StaticMeshRenderSystem");
#endif // _DEBUG

	auto query = World.Query<ETransform, EStaticMesh>();

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
		GET_RENDERER()->SubmitMesh(mesh.mesh, trans);
	}

#ifdef _DEBUG
	float systemTime = GET_TIMER()->EndReading("StaticMeshRenderSystem");

	ESystemDebugInfo debugInfo{};
	debugInfo.entityCount = query.GetCount();
	debugInfo.frameRunTime = systemTime;
	GET_ESYSTEMMANAGER()->RegisterFrameDebugInfo("Static Mesh Render", debugInfo);
#endif // _DEBUG
}
