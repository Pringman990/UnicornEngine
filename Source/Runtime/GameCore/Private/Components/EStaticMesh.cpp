#include "Components/EStaticMesh.h"
#include <Reflection/ReflectionRegistry.h>

REFL_DEFINE(EStaticMesh)
{
	refl::ClassBuilder<EStaticMesh>(STRINGIFYEXP(EStaticMesh), "afda9258-ee2e-49be-ae05-46c8cc74bd10")
		.Property("Mesh", &EStaticMesh::mesh)
			.Attribute("Mesh", refl::Attribute::DisplayName, "Mesh Asset");
}