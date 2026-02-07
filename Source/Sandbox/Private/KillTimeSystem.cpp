#include <Core.h>
#include <GameCore.h>
#include <ESystemManager.h>
#include <EWorld.h>

#include "KillTimerComponent.h"

void KillTimeSystem(EWorld& World)
{
	auto query = World.Query<KillTimerComponent>();

	for (uint32 i = 0; i < query.GetCount(); i++)
	{
		KillTimerComponent& killTimer = query.Get<KillTimerComponent>(i);

		float dt = Timer::Instance()->GetDeltaTime();

		killTimer.currentKillTime += dt;
		if (killTimer.currentKillTime >= killTimer.killTime)
		{
			World.DestroyEntity(query.GetEntity(i));
		}
	}
}

REGISTER_ESYSTEM(
	STRINGIFYEXP(KillTimeSystem),
	"acd10f23-00bc-4b64-bdaa-35d87bd93279",
	EPipeline::ESystemUpdate,
	EP_Simulation,
	KillTimeSystem
);