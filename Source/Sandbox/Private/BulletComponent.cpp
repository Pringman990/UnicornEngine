#include "BulletComponent.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(BulletComponent)
{
	refl::ClassBuilder<BulletComponent>(STRINGIFYEXP(BulletComponent), "38d74111-0bd7-4197-a50d-8e640f05ce39")
		.Property("Speed", &BulletComponent::speed);
}