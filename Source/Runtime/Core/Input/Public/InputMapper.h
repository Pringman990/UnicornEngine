#pragma once
#include "GenericDevice/IInputDevice.h"

enum class eInputActionType
{
	eReleased,
	ePressed,
	eHold
};

struct InputAction
{
	TFunction<void> callback;
	eInputActionType actionType;
};

struct InputActionMapping
{
	TUMap<uint32_t, TVector<InputAction*>> actions;
	bool hasActionToHandle = false;
};

class InputMapper
{
public:
	static InputMapper& GetInstance()
	{
		static InputMapper instance;
		return instance;
	}

	void CreateMapping(InputActionMapping& aActionMapping);
	void UnRegisterActionMapping(InputActionMapping& aActionMapping);

	void Init();

	TVector<InputActionMapping*>& _GetActionMappings();

private:
	InputMapper();
	~InputMapper();

private:
	TVector<InputActionMapping*> mActionsMappings;

	IInputDevice* mInputDevice;

	TUMap<FString, uint32_t> mNameToKey;
};