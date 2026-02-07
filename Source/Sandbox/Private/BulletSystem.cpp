#include <Core.h>
#include <GameCore.h>
#include <ESystemManager.h>
#include <EWorld.h>

#include "BulletComponent.h"
#include <Components/ETransform.h>

void BulletSystem(EWorld& World)
{
	auto query = World.Query<BulletComponent, ETransform>();

	for (uint32 i = 0; i < query.GetCount(); i++)
	{
		BulletComponent& bullet = query.Get<BulletComponent>(i);
		ETransform& trans = query.Get<ETransform>(i);

		float dt = Timer::Instance()->GetDeltaTime();

		trans.position.y += bullet.speed * dt;
	}
}

REGISTER_ESYSTEM(
	STRINGIFYEXP(BulletSystem),
	"acd10f21-00bc-4b64-bdaa-35d87bd93279",
	EPipeline::ESystemUpdate,
	EP_Simulation,
	BulletSystem
);