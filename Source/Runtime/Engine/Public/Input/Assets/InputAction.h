#pragma once
#include <Assets/AssetBase.h>
#include "Assets/AssetRef.h"

enum class InputActionType : uint8
{
	Bool = 0,
	Vec2 = 1
};

enum class InputActionTrigger : uint8
{
	Pressed = 0,
	Released = 1
};

class InputAction : public AssetBase
{
public:
	InputAction();
	InputAction(UniqueID128 UUID);
	~InputAction() override;

	InputActionType GetActionType() const { return mType; };
	void SetActionType(InputActionType Type) { mType = Type; };

	InputActionTrigger GetActionTrigger() const { return mTrigger; };
	void SetActionTrigger(InputActionTrigger Trigger) { mTrigger = Trigger; };

private:
	InputActionType mType;
	InputActionTrigger mTrigger;
};

REFL_DECLARE(InputAction)
REFL_DECLARE(AssetRef<InputAction>)