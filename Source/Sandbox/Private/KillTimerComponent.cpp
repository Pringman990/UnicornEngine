#include "KillTimerComponent.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(KillTimerComponent)
{
	refl::ClassBuilder<KillTimerComponent>(STRINGIFYEXP(KillTimerComponent), "d4594339-357e-4c70-b785-81e7e00bf844")
		.Property("Kill Time", &KillTimerComponent::killTime);
}