#include "PlayerMoveComponent.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(PlayerMoveComponent)
{
	refl::ClassBuilder<PlayerMoveComponent>(STRINGIFYEXP(PlayerMoveComponent), "bfda9258-fe2e-49be-ae05-46c8cc74bd10")
		.Property("Speed", &PlayerMoveComponent::speed);
}