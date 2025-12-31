#pragma once
#include <ComponentMinimal.h>
#include "MoveCubeComponent.meta.h"

struct MoveCubeComponent
{
	bool flip = false;
	float speedMultiplier = 1;
	float distance = 5;
};

REFL_DECLARE_COMPONENT(MoveCubeComponent);
