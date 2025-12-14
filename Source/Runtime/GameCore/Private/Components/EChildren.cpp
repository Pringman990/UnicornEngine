#include "Components/EChildren.h"

REFL_DEFINE(EChildren)
{
	refl::ClassBuilder<EChildren>(STRINGIFYEXP(EChildren), "b5126618-63c4-42b0-a363-c1271fa6d9be")
		.Property("Children", &EChildren::children);
}