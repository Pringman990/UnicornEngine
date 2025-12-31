#include "InputMapper.h"

InputMapper* InputMapper::sInstance = nullptr;
REGISTER_ENGINE_SUBSYSTEM(InputMapper)

InputMapper::InputMapper()
	:
	mInputDevice(InputDevice::Create(this))
{
#ifdef _DEBUG
	ASSERT(sInstance == nullptr, "The instance was not null and we are trying to set it again");
#endif
	sInstance = this;
}

InputMapper::~InputMapper()
{
	delete mInputDevice;
	mInputDevice = nullptr;

	if (sInstance == this)
		sInstance = nullptr;
}

ENGINE_API InputMapper* InputMapper::Instance()
{
#ifdef _DEBUG
	ASSERT(sInstance, "Instance was accessed before/after it was created/destroyed");
#endif
	return sInstance;
}

void InputMapper::Init()
{
	mInputDevice->Init();
}

void InputMapper::Update()
{
	mInputDevice->Update();
}

void InputMapper::CaptureMouse()
{
	mInputDevice->CaptureMouse();
}

void InputMapper::ReleaseMouse()
{
	mInputDevice->ReleaseMouse();
}

void InputMapper::HideMouse()
{
	mInputDevice->HideMouse();
}

void InputMapper::ShowMouse()
{
	mInputDevice->ShowMouse();
}

Vector2 InputMapper::GetMouseDelta()
{
	return mInputDevice->GetMouseDelta();
}

float InputMapper::GetMouseWheelDelta()
{
	return mInputDevice->GetMouseWheelDelta();
}
