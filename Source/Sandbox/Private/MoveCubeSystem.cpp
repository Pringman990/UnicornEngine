#include <Core.h>
#include <ESystemManager.h>
#include "MoveCubeComponent.h"
#include "Components/ETransform.h"

#include <EWorld.h>
#include <Timer/Timer.h>

//[[ESystemUpdate]]
void MoveCubeSystem(EWorld& World)
{
#ifdef _DEBUG
	GET_TIMER()->StartReading("MoveCubeSystem");
#endif // _DEBUG

	auto query = World.Query<ETransform, MoveCubeComponent>();

	for (uint32 i = 0; i < query.GetCount(); i++)
	{
		ETransform& transform = query.Get<ETransform>(i);
		MoveCubeComponent& moveComp = query.Get<MoveCubeComponent>(i);

		if (transform.position.x > moveComp.distance)
			moveComp.flip = true;
		else if (transform.position.x < 0)
			moveComp.flip = false;

		Vector3 newPos = transform.position;
		if (moveComp.flip)
			newPos.x -= GET_TIMER()->GetDeltaTime() * moveComp.speedMultiplier;
		else
			newPos.x += GET_TIMER()->GetDeltaTime() * moveComp.speedMultiplier;
	
		transform.position = newPos;
	}

#ifdef _DEBUG
	float systemTime = GET_TIMER()->EndReading("MoveCubeSystem");

	ESystemDebugInfo debugInfo{};
	debugInfo.entityCount = query.GetCount();
	debugInfo.frameRunTime = systemTime;
	GET_ESYSTEMMANAGER()->RegisterFrameDebugInfo("Move Cube", debugInfo);
#endif // _DEBUG
}