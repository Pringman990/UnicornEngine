#include "EditorPch.h"
#include "EditorCamera.h"

#include <../GraphicsEngine/source/Camera.h>
#include <../GraphicsEngine/source/GraphicsEngine.h>
#include <../Engine/source/input/InputManager.h>
#include <../Engine/source/Engine.h>

EditorCamera::EditorCamera()
	:
	mMoveSpeed(10),
	mRotationSpeed(1)
{
}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::Update(float aDeltaTime)
{

	if (!Input::IsKeyHeld(VK_RBUTTON))
	{
		Input::ReleaseMouse();
		return;
	}

	std::shared_ptr<Camera> camera = GraphicsEngine::GetInstance().GetCamera();
	Transform& transform = camera->GetTransform();
	Matrix matrix = transform.GetMatrix();
	Vector3 position = transform.GetPosition();
	Vector3 right = matrix.Right();
	Vector3 up = matrix.Up();
	Vector3 forward = matrix.Forward();

	float realMovementSpeed = mMoveSpeed * aDeltaTime;

	if (Input::IsKeyHeld('W'))
	{
		position = position + (forward * -realMovementSpeed);
	}
	if (Input::IsKeyHeld('S'))
	{
		position = position + (forward * realMovementSpeed);
	}
	if (Input::IsKeyHeld('D'))
	{
		position = position + (right * realMovementSpeed);
	}
	if (Input::IsKeyHeld('A'))
	{
		position = position + (right * -realMovementSpeed);
	}
	if (Input::IsKeyHeld('E'))
	{
		position = position + (up * realMovementSpeed);
	}
	if (Input::IsKeyHeld('Q'))
	{
		position = position + (up * -realMovementSpeed);
	}

	Vector2 dPos = Input::GetMouseDelta();
	if (dPos.x != 0 || dPos.y != 0)
	{
		float realRotationSpeed = mRotationSpeed * aDeltaTime;
		float y = transform.GetEularRotation().y + (realRotationSpeed * dPos.x);
		float x = transform.GetEularRotation().x + (realRotationSpeed * dPos.y);

		constexpr float maxPitchAngle = 3.14159265359f / 2.0f - 0.01f;
		x = CLAMP(x, -maxPitchAngle, maxPitchAngle);
		transform.SetRotation({x,y,0});
	}
	transform.SetPosition(position);
	Input::CaptureMouse();
}
