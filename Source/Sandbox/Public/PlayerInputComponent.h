#pragma once
#include <ComponentMinimal.h>
#include <Input/Assets/InputMapping.h>

struct PlayerInputComponent
{
	Vector<AssetRef<InputMapping>> mappings;

	AssetRef<InputAction> moveAction;
};

REFL_DECLARE_COMPONENT(PlayerInputComponent);