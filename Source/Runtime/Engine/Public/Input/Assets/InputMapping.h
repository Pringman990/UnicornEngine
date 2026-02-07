#pragma once
#include <Assets/AssetBase.h>
#include "Assets/AssetRef.h"

#include "InputAction.h"

struct InputBinding
{
	enum class Modifier : uint8
	{
		Negate = 0
	};

	String key;
	Vector<Modifier> modifiers;
};

class InputMapping : public AssetBase
{
public:
	InputMapping();
	InputMapping(UniqueID128 UUID);
	~InputMapping() override;

	void AddBinding(AssetRef<InputAction> Action, InputBinding Binding);

private:
	UnorderedMap<UniqueID128/*AssetRef<InputAction>*/, Vector<InputBinding>> mBindings;
};

REFL_DECLARE(InputMapping)
REFL_DECLARE(AssetRef<InputMapping>)