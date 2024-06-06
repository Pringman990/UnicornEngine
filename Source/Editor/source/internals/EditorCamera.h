#pragma once

class EditorCamera
{
public:
	EditorCamera();
	~EditorCamera();

	void Update(float aDeltaTime);

private:
	float mMoveSpeed;
	float mRotationSpeed;
};