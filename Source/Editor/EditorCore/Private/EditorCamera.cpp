#include "pch.h"
#include "EditorCamera.h"

#include <Camera.h>
#include <Renderer.h>

EditorCamera::EditorCamera()
	:
	mMoveSpeed(10),
	mRotationSpeed(1),
	mCamera(Renderer::GetInstance()->GetActiveCamera())
{
}

EditorCamera::~EditorCamera()
{
}

void EditorCamera::Update(float aDeltaTime)
{
	if (!ImGui::IsKeyDown(ImGuiKey_MouseRight))
	{
		InputMapper::GetInstance()->ReleaseMouse();
		return;
	}

	Transform& transform = mCamera->GetTransform();
	Matrix matrix = transform.GetMatrix();
	Vector3 position = transform.GetPosition();
	Vector3 right = matrix.Right();
	Vector3 up = matrix.Up();
	Vector3 forward = matrix.Forward();

	float realMovementSpeed = mMoveSpeed * aDeltaTime;

	if (ImGui::IsKeyDown(ImGuiKey_W))
	{
		position = position + (forward * -realMovementSpeed);
	}
	if (ImGui::IsKeyDown(ImGuiKey_S))
	{
		position = position + (forward * realMovementSpeed);
	}
	if (ImGui::IsKeyDown(ImGuiKey_D))
	{
		position = position + (right * realMovementSpeed);
	}
	if (ImGui::IsKeyDown(ImGuiKey_A))
	{
		position = position + (right * -realMovementSpeed);
	}
	if (ImGui::IsKeyDown(ImGuiKey_E))
	{
		position = position + (up * realMovementSpeed);
	}
	if (ImGui::IsKeyDown(ImGuiKey_Q))
	{
		position = position + (up * -realMovementSpeed);
	}
	
	Vector3 currentRotation = transform.GetEularRotation();
	Vector3 targetRotation = currentRotation;

	Vector2 dPos = InputMapper::GetInstance()->GetMouseDelta();
	if (dPos.x != 0 || dPos.y != 0)
	{
		float realRotationSpeed = mRotationSpeed * aDeltaTime;

		targetRotation.y += realRotationSpeed * dPos.x;
		targetRotation.x += realRotationSpeed * dPos.y;
		
		constexpr float maxPitchAngle = 3.14159265359f / 2.0f - 0.01f;
		targetRotation.x = CLAMP(targetRotation.x, -maxPitchAngle, maxPitchAngle);
	}

	float lerpSpeed = 30.0f;
	Vector3 smoothedRotation = {};
	smoothedRotation.x = std::lerp(currentRotation.x, targetRotation.x, lerpSpeed * aDeltaTime);
	smoothedRotation.y = std::lerp(currentRotation.y, targetRotation.y, lerpSpeed * aDeltaTime);
	smoothedRotation.z = 0.0f;
	transform.SetRotation(smoothedRotation);

	transform.SetPosition(position);
	InputMapper::GetInstance()->CaptureMouse();
}

Camera* EditorCamera::GetCamera()
{
	return mCamera;
}
