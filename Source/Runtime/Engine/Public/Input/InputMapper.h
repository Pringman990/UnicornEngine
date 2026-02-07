#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include <StandardTypes/StandardTypes.h>
#include "GenericDevice/InputDevice.h"

enum class InputSourceType
{
	KeyboardMouse,
	//Gamepad,
	//Network,
	//AI
};

struct InputSource
{
	//InputSourceID id;
	InputSourceType type;

	bool connected;
	uint64 deviceUID;
};

/*
* Engine subsystem
*/
class InputMapper final 
{
	friend struct subsystem::SubsystemDescriptor;

public:

	ENGINE_API static InputMapper* Instance();

	ENGINE_API void Init();
	ENGINE_API void Update();
			   
	ENGINE_API void CaptureMouse();
	ENGINE_API void ReleaseMouse();
			   
	ENGINE_API void HideMouse();
	ENGINE_API void ShowMouse();

	ENGINE_API Vector2 GetMouseDelta();
	ENGINE_API float GetMouseWheelDelta();

private:
	InputMapper();
	~InputMapper();

private:
	static InputMapper* sInstance;

	InputDevice* mInputDevice;
	UnorderedMap<String, uint32> mNameToKey;

	//struct DeviceEntry
	//{
	//	OwnedPtr<InputDevice> device;
	//	InputSourceID source;
	//};

	//UnorderedMap<uint64, DeviceEntry> mUIDToDevice;
	//UnorderedMap<InputSourceID, InputSource> mSources;
};