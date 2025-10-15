#pragma once
#include <EngineMinimal.h>

struct alignas(16) CameraUBO
{
	Matrix viewMatrix;
	Matrix projMatrix;
	Vector3 cameraPosition;
	uint32 pad;
};

class Camera
{
public:
	bool operator!=(const Camera& Other)
	{
		return (Other.mTransform != this->mTransform);
	}
public:
	Camera();
	~Camera();

	void SetPerspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ);
	void SetOrthographic(Vector2 Resolution, float NearZ, float FarZ);

	Transform& GetTransform();
	Vector3 GetPosition();
	Matrix& GetProjectionMatrix();
	Matrix GetViewMatrix();

	void GetFarNearPlanes(float& OUT Far, float& OUT Near);
	Matrix GetClipSpaceMatrix();
private:
	Transform mTransform;
	Matrix mProjectionMatrix;
	float mFarPlane;
	float mNearPlane;
	float mFov;
	float mAspectRatio;

};
