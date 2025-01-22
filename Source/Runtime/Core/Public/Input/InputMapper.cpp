#include "pch.h"
#include "InputMapper.h"

InputMapper::InputMapper()
	:
	mInputDevice(InputDevice::Create(this))
{
}

InputMapper::~InputMapper()
{
	delete mInputDevice;
	mInputDevice = nullptr;
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
