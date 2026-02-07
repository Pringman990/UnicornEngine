#include "ShootComponent.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(ShootComponent)
{
	refl::ClassBuilder<ShootComponent>(STRINGIFYEXP(ShootComponent), "3016fbb2-3dc5-48b5-9ecd-b3cdf6cab7f3")
		.Property("Shoot Speed", &ShootComponent::shootSpeed);
}