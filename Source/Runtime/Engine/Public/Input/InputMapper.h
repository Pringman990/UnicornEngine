#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include <StandardTypes/StandardTypes.h>
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

/*
* Engine subsystem
*/
class InputMapper final 
{
	friend struct subsystem::SubsystemDescriptor;
	//using Key = USHORT;

public:
	//void CreateMapping(InputActionMap& aActionMapping);
	//void UnRegisterActionMapping(InputActionMap& aActionMapping);
	//
	//void Init();
	//
	//TVector<InputActionMap*>& _GetActionMappings();

	ENGINE_API void Init();
	ENGINE_API void Update();
			   
	ENGINE_API void CaptureMouse();
	ENGINE_API void ReleaseMouse();
			   
	ENGINE_API void HideMouse();
	ENGINE_API void ShowMouse();

	ENGINE_API Vector2 GetMouseDelta();

private:
	InputMapper();
	~InputMapper();

private:
	InputDevice* mInputDevice;
	UnorderedMap<String, uint32> mNameToKey;
};