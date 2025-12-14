#include "MoveCubeComponent.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(MoveCubeComponent)
{
	refl::ClassBuilder<MoveCubeComponent>(STRINGIFYEXP(MoveCubeComponent), "bfda9258-ee2e-49be-ae05-46c8cc74bd10")
		.Property("Flip", &MoveCubeComponent::flip)
			.Attribute("Flip", refl::Attribute::Private)
		.Property("Speed", &MoveCubeComponent::speedMultiplier)
		.Property("a", &MoveCubeComponent::a)
		.Property("Distance", &MoveCubeComponent::distance);
}