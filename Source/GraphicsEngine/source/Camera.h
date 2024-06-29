#pragma once

class Camera
{
public:
	Camera();
	~Camera();

	void SetPerspective(float aFovAngleY, float aAspectRatio, float aNearZ, float aFarZ);
	void SetOrthographic(Vector2 aResolution, float aNearZ, float aFarZ);

	Transform& GetTransform() { return mTransform; };
	Matrix& GetProjectionMatrix() { return mProjectionMatrix; };

	void GetFarNearPlanes(float& OUT aFar, float& OUT aNear) { aFar = mFarPlane; aNear = mNearPlane; };
	Matrix GetClipSpaceMatrix();
private:
	Transform mTransform;
	Matrix mProjectionMatrix;
	float mFarPlane;
	float mNearPlane;
	float mFov;
	float mAspectRatio;
};