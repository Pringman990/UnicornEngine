#pragma once
#include <Core.h>

class Camera;

class EditorCamera
{
public:
	EditorCamera();
	~EditorCamera();

	void Update(float DeltaTime);
	Camera* GetCamera();
private:
	float mMoveSpeed;
	float mRotationSpeed;

	Camera* mCamera;
};