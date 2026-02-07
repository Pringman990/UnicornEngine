#include "Input/Assets/InputMapping.h"

REFL_DEFINE(InputMapping)
{
	refl::ClassBuilder<InputMapping>(STRINGIFYEXP(InputMapping), "613c9ab4-e3eb-4e39-89ee-a28f365ce91c");
}

REFL_DEFINE(AssetRef<InputMapping>)
{
	refl::ClassBuilder<AssetRef<InputMapping>>(STRINGIFYEXP(AssetRef<InputMapping>), "661c9ab4-e3eb-4e39-89ee-a28f365ce91c")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				SaveAssetRef<InputMapping>(obj, archive, key);
			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				LoadAssetRef<InputMapping>(obj, archive, key);
				return true;
			});
}

InputMapping::InputMapping()
	:
	AssetBase({})
{
}

InputMapping::InputMapping(UniqueID128 UUID)
	:
	AssetBase(UUID)
{
}

InputMapping::~InputMapping()
{
}

void InputMapping::AddBinding(AssetRef<InputAction> Action, InputBinding Binding)
{
	if (!Action)
	{
		LOG_ERROR("Tried binding invalid input action");
		return;
	}

	mBindings[Action.Get()->GetUUID()].push_back(std::move(Binding));
}
