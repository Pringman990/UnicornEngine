#include "Components/ETransform.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(ETransform)
{
	refl::ClassBuilder<ETransform>(STRINGIFYEXP(ETransform), "afdb9258-ee2e-49be-ae05-46c8cc74bd10")
		.Property("Position", &ETransform::position)
		.Property("Rotation", &ETransform::rotation)
		.Property("Scale", &ETransform::scale);
}