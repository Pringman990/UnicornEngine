#include "FreeLookCamera.h"

#include "Input/InputMapper.h"

FreeLookCamera::FreeLookCamera()
	:
	mMoveSpeed(2),
	mBoostMoveSpeed(10),
	mRotateSpeed(2),
	mZoomSpeed(1),
	mSchema(FreeLookCameraControlSchema::FreeLook)
{
}

FreeLookCamera::~FreeLookCamera()
{
}

void FreeLookCamera::HandleInputsAndMove()
{
	switch (mSchema)
	{
	case FreeLookCameraControlSchema::FreeLook:
		HandleFreeLook();
		break;
	case FreeLookCameraControlSchema::OrthoPan:
		HandleOrthoPan();
		break;
	default:
		break;
	}	
}

void FreeLookCamera::HandleFreeLook()
{
	InputMapper* input = InputMapper::Instance();

	if (!GetAsyncKeyState(VK_RBUTTON))
	{
		input->ReleaseMouse();
		return;
	}

	Timer* timer = Timer::Instance();

	Matrix matrix = mTransform.GetMatrix();
	Vector3 position = mTransform.GetPosition();
	Vector3 right = matrix.Right();
	Vector3 up = matrix.Up();
	Vector3 forward = matrix.Forward();

	float realMovementSpeed = 5 * timer->GetDeltaTime();

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		realMovementSpeed = 10 * timer->GetDeltaTime();
	}

	if (GetAsyncKeyState('W'))
	{
		position = position + (forward * -realMovementSpeed);
	}
	if (GetAsyncKeyState('S'))
	{
		position = position + (forward * realMovementSpeed);
	}
	if (GetAsyncKeyState('D'))
	{
		position = position + (right * realMovementSpeed);
	}
	if (GetAsyncKeyState('A'))
	{
		position = position + (right * -realMovementSpeed);
	}
	if (GetAsyncKeyState('E'))
	{
		position = position + (up * realMovementSpeed);
	}
	if (GetAsyncKeyState('Q'))
	{
		position = position + (up * -realMovementSpeed);
	}

	Vector3 currentRotation = mTransform.GetEularRadRotation();
	Vector3 targetRotation = currentRotation;
	Vector2 dPos = input->GetMouseDelta();
	if (dPos.x != 0 || dPos.y != 0)
	{
		float realRotationSpeed = 10 * timer->GetDeltaTime();

		targetRotation.y += realRotationSpeed * dPos.x;
		targetRotation.x += realRotationSpeed * dPos.y;

		constexpr float maxPitchAngle = 3.14159265359f / 2.0f - 0.01f;
		targetRotation.x = CLAMP(targetRotation.x, -maxPitchAngle, maxPitchAngle);
	}

	mTransform.SetRotation(targetRotation.GetDegree());

	mTransform.SetPosition(position);
	input->CaptureMouse();
}

void FreeLookCamera::HandleOrthoPan()
{
	InputMapper* input = InputMapper::Instance();
	Timer* timer = Timer::Instance();

	if (!GetAsyncKeyState(VK_RBUTTON))
	{
		input->ReleaseMouse();
		return;
	}

	HandleZoom();

	Vector3 position = mTransform.GetPosition();
	float moveSpeed = mMoveSpeed * timer->GetDeltaTime();

	if (GetAsyncKeyState('W')) 
		position.y += moveSpeed;
	
	if (GetAsyncKeyState('S'))
		position.y -= moveSpeed;
	
	if (GetAsyncKeyState('A'))
		position.x -= moveSpeed;
	
	if (GetAsyncKeyState('D'))
		position.x += moveSpeed;

	Vector2 delta = input->GetMouseDelta();
	position.x += delta.x * 0.05f;
	position.y -= delta.y * 0.05f;

	mTransform.SetPosition(position);
	input->CaptureMouse();
}

void FreeLookCamera::HandleZoom()
{
	float scroll = InputMapper::Instance()->GetMouseWheelDelta();
	if (scroll == 0.0f) 
		return;

	float orthoSize = mOrtoSize;
	orthoSize -= scroll * mZoomSpeed;
	orthoSize = max(orthoSize, 1);

	SetOrthoSize(orthoSize);
}
