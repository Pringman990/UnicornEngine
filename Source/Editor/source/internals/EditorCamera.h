#pragma once

class Camera;

class EditorCamera
{
public:
	EditorCamera();
	~EditorCamera();

	void Update(float aDeltaTime);

	std::shared_ptr<Camera> GetCamera();
	
private:
	float mMoveSpeed;
	float mRotationSpeed;

	std::shared_ptr<Camera> mCamera;
};