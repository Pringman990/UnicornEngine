#pragma once
#include <Singleton.h>
#include "GenericDevice/InputDevice.h"

enum class eInputActionType
{
	eReleased,
	ePressed,
	eHold
};

struct InputAction
{
};

struct InputActionMap
{
};

class InputMapper final : public Singleton<InputMapper>
{
	//using Key = USHORT;

public:
	//void CreateMapping(InputActionMap& aActionMapping);
	//void UnRegisterActionMapping(InputActionMap& aActionMapping);
	//
	//void Init();
	//
	//TVector<InputActionMap*>& _GetActionMappings();

	void Init();
	void Update();

	void CaptureMouse();
	void ReleaseMouse();

	void HideMouse();
	void ShowMouse();

	Vector2 GetMouseDelta();

private:
	friend class Singleton<InputMapper>;
	InputMapper();
	~InputMapper();

private:
	InputDevice* mInputDevice;
	UnorderedMap<String, uint32> mNameToKey;
};