#include "Components/EParent.h"

REFL_DEFINE(EParent)
{
	refl::ClassBuilder<EParent>(STRINGIFYEXP(EParent), "afda9228-ee2e-49be-ae05-46c8cc74bd10")
		.Property("Parent", &EParent::parent);
}