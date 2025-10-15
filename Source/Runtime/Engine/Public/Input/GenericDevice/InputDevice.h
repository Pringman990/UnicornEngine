#pragma once
#include <StandardTypes/StandardTypes.h>
#include <SimpleMath.h>

class InputMapper;

class InputDevice
{
public:
	InputDevice(InputMapper* Mapper) : mInputMapper(Mapper) {};
	virtual ~InputDevice() {};
	static InputDevice* Create(InputMapper* Mapper);

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void GetKeys(UnorderedMap<String, uint32>& StringKeyMap) = 0;

	virtual void CaptureMouse() = 0;
	virtual void ReleaseMouse() = 0;

	virtual void HideMouse() = 0;
	virtual void ShowMouse() = 0;

	virtual Vector2 GetMouseDelta() = 0;

protected:
	InputMapper* mInputMapper;
};