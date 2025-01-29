#pragma once
#include <Math/Transform.h>

class Camera
{
public:
	bool operator!=(const Camera& aOther)
	{
		return (aOther.mTransform != this->mTransform);
	}
public:
	Camera();
	~Camera();

	void SetPerspective(float aFovAngleY, float aAspectRatio, float aNearZ, float aFarZ);
	void SetOrthographic(Vector2 aResolution, float aNearZ, float aFarZ);

	Transform& GetTransform();
	Matrix& GetProjectionMatrix();

	void GetFarNearPlanes(float& OUT aFar, float& OUT aNear);
	Matrix GetClipSpaceMatrix();
private:
	Transform mTransform;
	Matrix mProjectionMatrix;
	float mFarPlane;
	float mNearPlane;
	float mFov;
	float mAspectRatio;

};
