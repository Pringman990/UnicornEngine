#include "Components/ENameComponent.h"

REFL_DEFINE(ENameComponent)
{
	refl::ClassBuilder<ENameComponent>("Name Component", "1743b14b-3df0-4ed0-b2c6-85d3be17651c")
		.Property("Name", &ENameComponent::name);
}