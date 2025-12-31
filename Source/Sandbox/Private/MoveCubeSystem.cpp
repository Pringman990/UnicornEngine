#include <Core.h>
#include <GameCore.h>
#include <ESystemManager.h>
#include "MoveCubeComponent.h"
#include "Components/ETransform.h"

#include <EWorld.h>
#include <Timer/Timer.h>

void MoveCubeSystem(EWorld& World)
{
#ifdef _DEBUG
	Timer::Instance()->StartReading("MoveCubeSystem");
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
			newPos.x -= Timer::Instance()->GetDeltaTime() * moveComp.speedMultiplier;
		else
			newPos.x += Timer::Instance()->GetDeltaTime() * moveComp.speedMultiplier;
	
		transform.position = newPos;
	}

#ifdef _DEBUG
	float systemTime = Timer::Instance()->EndReading("MoveCubeSystem");

	ESystemDebugInfo debugInfo{};
	debugInfo.entityCount = query.GetCount();
	debugInfo.frameRunTime = systemTime;
	ESystemManager::Instance()->RegisterFrameDebugInfo("MoveCubeSystem", debugInfo);
#endif // _DEBUG
}

REGISTER_ESYSTEM(
	STRINGIFYEXP(MoveCubeSystem),
	"fcd60f23-00bc-4b84-bdaa-35d87bd93279",
	EPipeline::ESystemUpdate,
	EP_Simulation,
	MoveCubeSystem
	);