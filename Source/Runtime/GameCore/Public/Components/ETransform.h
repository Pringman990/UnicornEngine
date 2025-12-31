#pragma once 
#include <EngineMinimal.h>

struct ETransform
{
	Vector3 position;
	Vector3 rotation;
	Vector3 scale = {1,1,1};
};

REFL_DECLARE_COMPONENT(ETransform)