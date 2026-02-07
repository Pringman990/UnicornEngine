#include <Core.h>
#include <GameCore.h>
#include <ESystemManager.h>
#include <EWorld.h>
#include <Input/InputMapper.h>

#include "ShootComponent.h"
#include "KillTimerComponent.h"
#include "BulletComponent.h"
#include <Components/ETransform.h>
#include <Components/ENameComponent.h>
#include <Components/EStaticMesh.h>

void ShootSystem(EWorld& World)
{
	auto query = World.Query<ETransform, ShootComponent>();

	for (uint32 i = 0; i < query.GetCount(); i++)
	{
		ETransform& transform = query.Get<ETransform>(i);
		ShootComponent& shootComp = query.Get<ShootComponent>(i);

		float dt = Timer::Instance()->GetDeltaTime();

		shootComp.currentShootTime += dt;
		if (GetAsyncKeyState(VK_RBUTTON) && shootComp.currentShootTime >= shootComp.shootSpeed)
		{
			EEntity shoot = World.CreateEntity();
			ETransform* trans = World.AddComponent<ETransform>(shoot);
			trans->scale = Vector3(0.2f, 0.6f, 0.2f);
			trans->position = transform.position;
			trans->position.y += (transform.scale.y / 2);

			World.AddComponent<ENameComponent>(shoot);
			EStaticMesh* mesh = World.AddComponent<EStaticMesh>(shoot);
			mesh->mesh = AssetRegistry::Instance()->Load<Mesh>("engine://Models/sm_cube.asset");

			World.AddComponent<KillTimerComponent>(shoot);
			World.AddComponent<BulletComponent>(shoot);

			shootComp.currentShootTime = 0;
		}
	}
}

REGISTER_ESYSTEM(
	STRINGIFYEXP(ShootSystem),
	"acd60f23-00bc-4b64-bdaa-35d87bd93279",
	EPipeline::ESystemUpdate,
	EP_Simulation,
	ShootSystem
);