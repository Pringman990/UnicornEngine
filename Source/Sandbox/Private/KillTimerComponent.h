#pragma once
#include <ComponentMinimal.h>

struct KillTimerComponent
{
	float killTime = 1;
	float currentKillTime = 0;
};

REFL_DECLARE_COMPONENT(KillTimerComponent);
