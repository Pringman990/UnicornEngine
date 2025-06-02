#include "GameWorld.h"

#include <StandardTypes/StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <Logger/Logger.h>

#include <RendererCore.h>
#include <d3dcompiler.h>

namespace {
	GameWorld gameworld;

	Vertex verticies[] =
	{
		// Front face
	{ { -1, -1, -1, 1 }, { 1, 0, 0 } }, // Bottom-left-front (red)
	{ { -1,  1, -1, 1 }, { 1, 0, 0 } }, // Top-left-front
	{ {  1,  1, -1, 1 }, { 1, 0, 0 } }, // Top-right-front
	{ {  1, -1, -1, 1 }, { 1, 0, 0 } }, // Bottom-right-front

	// Back face
	{ { -1, -1,  1, 1 }, { 0, 1, 0 } }, // Bottom-left-back (green)
	{ {  1, -1,  1, 1 }, { 0, 1, 0 } }, // Bottom-right-back
	{ {  1,  1,  1, 1 }, { 0, 1, 0 } }, // Top-right-back
	{ { -1,  1,  1, 1 }, { 0, 1, 0 } }, // Top-left-back

	// Left face
	{ { -1, -1,  1, 1 }, { 0, 0, 1 } }, // Bottom-left-back (blue)
	{ { -1,  1,  1, 1 }, { 0, 0, 1 } }, // Top-left-back
	{ { -1,  1, -1, 1 }, { 0, 0, 1 } }, // Top-left-front
	{ { -1, -1, -1, 1 }, { 0, 0, 1 } }, // Bottom-left-front

	// Right face
	{ { 1, -1, -1, 1 }, { 1, 1, 0 } }, // Bottom-right-front (yellow)
	{ { 1,  1, -1, 1 }, { 1, 1, 0 } }, // Top-right-front
	{ { 1,  1,  1, 1 }, { 1, 1, 0 } }, // Top-right-back
	{ { 1, -1,  1, 1 }, { 1, 1, 0 } }, // Bottom-right-back

	// Top face
	{ { -1, 1, -1, 1 }, { 1, 0, 1 } }, // Top-left-front (magenta)
	{ { -1, 1,  1, 1 }, { 1, 0, 1 } }, // Top-left-back
	{ {  1, 1,  1, 1 }, { 1, 0, 1 } }, // Top-right-back
	{ {  1, 1, -1, 1 }, { 1, 0, 1 } }, // Top-right-front

	// Bottom face
	{ { -1, -1, -1, 1 }, { 0, 1, 1 } }, // Bottom-left-front (cyan)
	{ {  1, -1, -1, 1 }, { 0, 1, 1 } }, // Bottom-right-front
	{ {  1, -1,  1, 1 }, { 0, 1, 1 } }, // Bottom-right-back
	{ { -1, -1,  1, 1 }, { 0, 1, 1 } }, // Bottom-left-back
	};

	uint16_t cubeIndices[] = {
		// Front face
		0, 1, 2,
		0, 2, 3,

		// Back face
		4, 5, 6,
		4, 6, 7,

		// Left face
		8, 9, 10,
		8, 10, 11,

		// Right face
		12, 13, 14,
		12, 14, 15,

		// Top face
		16, 17, 18,
		16, 18, 19,

		// Bottom face
		20, 21, 22,
		20, 22, 23,
	};
}

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	LOG_INFO("GameWorld init...");

	//float aspect = (16.f / 9.f);
	//mCamera.SetPerspective(90, aspect, 0.01f, 1000.f);
	//mCamera.GetTransform().SetPosition({ 0,0,-5 });
	//Renderer::GetInstance()->SetMainCamera(&mCamera);


	
}

void GameWorld::Render()
{
	//auto cmdList = Renderer::GetInstance()->GetMainCommandList();


}

void GameWorld::UpdateCamera()
{
	if (!GetAsyncKeyState(VK_RBUTTON))
	{
		InputMapper::GetInstance()->ReleaseMouse();
		return;
	}

	Transform& transform = mCamera.GetTransform();
	Matrix matrix = transform.GetMatrix();
	Vector3 position = transform.GetPosition();
	Vector3 right = matrix.Right();
	Vector3 up = matrix.Up();
	Vector3 forward = matrix.Forward();

	float realMovementSpeed = 5 * Timer::GetInstance()->GetDeltaTime();

	if (GetAsyncKeyState(VK_LSHIFT))
	{
		realMovementSpeed = 10 * Timer::GetInstance()->GetDeltaTime();
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

	Vector3 currentRotation = transform.GetEularRotation();
	Vector3 targetRotation = currentRotation;
	Vector2 dPos = InputMapper::GetInstance()->GetMouseDelta();
	if (dPos.x != 0 || dPos.y != 0)
	{
		float realRotationSpeed = 1 * Timer::GetInstance()->GetDeltaTime();

		targetRotation.y += realRotationSpeed * dPos.x;
		targetRotation.x += realRotationSpeed * dPos.y;

		constexpr float maxPitchAngle = 3.14159265359f / 2.0f - 0.01f;
		targetRotation.x = CLAMP(targetRotation.x, -maxPitchAngle, maxPitchAngle);
	}

	float lerpSpeed = 30.0f;
	Vector3 smoothedRotation = {};
	smoothedRotation.x = std::lerp(currentRotation.x, targetRotation.x, lerpSpeed * Timer::GetInstance()->GetDeltaTime());
	smoothedRotation.y = std::lerp(currentRotation.y, targetRotation.y, lerpSpeed * Timer::GetInstance()->GetDeltaTime());
	smoothedRotation.z = 0.0f;
	transform.SetRotation(smoothedRotation);

	transform.SetPosition(position);
	InputMapper::GetInstance()->CaptureMouse();
}

void InitGameWorld()
{
	gameworld.Init();
}

void RenderGameWorld()
{
	gameworld.Render();
}
