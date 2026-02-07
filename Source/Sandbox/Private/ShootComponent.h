#pragma once
#include <ComponentMinimal.h>

struct ShootComponent
{
	float shootSpeed = 0.5f;
	float currentShootTime = 0;
};

REFL_DECLARE_COMPONENT(ShootComponent);