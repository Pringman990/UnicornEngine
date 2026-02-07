#include "PlayerInputComponent.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(PlayerInputComponent)
{
	refl::ClassBuilder<PlayerInputComponent>(STRINGIFYEXP(PlayerInputComponent), "afda9258-fe2e-49be-ae05-46c8cc74bd10")
		.Property("Mappings", &PlayerInputComponent::mappings)
		.Property("Move Action", &PlayerInputComponent::moveAction);
}