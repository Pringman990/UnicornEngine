#include "pch.h"
#include "InputMapper.h"

InputMapper::InputMapper()
	:
	mInputDevice(IInputDevice::Create(this))
{
}

InputMapper::~InputMapper()
{
	delete mInputDevice;
	mInputDevice = nullptr;
}

void InputMapper::CreateMapping(InputActionMapping& aActionMapping)
{
	mActionsMappings.AddUnique(&aActionMapping);
}

void InputMapper::UnRegisterActionMapping(InputActionMapping& aActionMapping)
{
	mActionsMappings.Erase(&aActionMapping);
}

void InputMapper::Init()
{
	mInputDevice->Init();
	mInputDevice->GetKeys(mNameToKey);
}

TVector<InputActionMapping*>& InputMapper::_GetActionMappings()
{
	return mActionsMappings;
}