#include <Core.h>
#include <GameCore.h>
#include <ESystemManager.h>
#include <EWorld.h>

#include "PlayerMoveComponent.h"
#include <Components/ETransform.h>

void PlayerMoveSystem(EWorld& World)
{
	auto query = World.Query<ETransform, PlayerMoveComponent>();

	for (uint32 i = 0; i < query.GetCount(); i++)
	{
		ETransform& transform = query.Get<ETransform>(i);
		PlayerMoveComponent& playerMove = query.Get<PlayerMoveComponent>(i);

		float dt = Timer::Instance()->GetDeltaTime();

		if (GetAsyncKeyState('W'))
			transform.position.y += playerMove.speed * dt;
		if (GetAsyncKeyState('S'))
			transform.position.y -= playerMove.speed * dt;
		if (GetAsyncKeyState('A'))
			transform.position.x -= playerMove.speed * dt;
		if (GetAsyncKeyState('D'))
			transform.position.x += playerMove.speed * dt;
	}
}

REGISTER_ESYSTEM(
	STRINGIFYEXP(PlayerMoveSystem),
	"fcd60f23-00bc-4b64-bdaa-35d87bd93279",
	EPipeline::ESystemUpdate,
	EP_Simulation,
	PlayerMoveSystem
);