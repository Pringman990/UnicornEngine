#pragma once

class InputMapper;

class IInputDevice
{
public:
	IInputDevice(InputMapper* aMapper) : mInputMapper(aMapper) {};
	virtual ~IInputDevice() {};

	virtual void Init() {};

	static IInputDevice* Create(InputMapper* aMapper);
	virtual void GetKeys(TUMap<FString, uint32_t>& aStringKeyMap);

protected:
	InputMapper* mInputMapper;
};