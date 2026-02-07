#include "Input/Assets/InputAction.h"


REFL_DEFINE(InputAction)
{
	refl::ClassBuilder<InputAction>(STRINGIFYEXP(InputAction), "623c9ab4-e3eb-4e39-89ee-a28f365ce91c");
}

REFL_DEFINE(AssetRef<InputAction>)
{
	refl::ClassBuilder<AssetRef<InputAction>>(STRINGIFYEXP(AssetRef<InputAction>), "621c9ab4-e3eb-4e39-89ee-a28f365ce91c")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				SaveAssetRef<InputAction>(obj, archive, key);
			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				LoadAssetRef<InputAction>(obj, archive, key);
				return true;
			});
}

InputAction::InputAction()
	:
	AssetBase({})
{
}

InputAction::InputAction(UniqueID128 UUID)
	:
	AssetBase(UUID)
{
}

InputAction::~InputAction()
{
}
