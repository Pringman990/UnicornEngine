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
	enum class CameraPerspective
	{
		Undefined,
		Perspective,
		Orthographic
	};

public:
	bool operator!=(const Camera& Other)
	{
		return (Other.mTransform != this->mTransform);
	}
public:
	Camera();
	virtual ~Camera();

	void SetPerspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ);
	void SetOrthographic(Vector2 Resolution, float NearZ, float FarZ);
	void SetOrthoSize(float Size);

	void KeepAspect(bool Keep) { mKeepAspect = Keep; };

	Transform& GetTransform();
	Vector3 GetPosition();
	Matrix& GetProjectionMatrix();
	Matrix GetViewMatrix();
	float GetOrtoSize() const { return mOrtoSize; };

	void GetFarNearPlanes(float& OUT Far, float& OUT Near);
	Matrix GetClipSpaceMatrix();

	/**
	* Will not resize if KeepAspect is true.
	*/
	void HandleResizeEvent(int32 Width, int32 Height);
protected:

protected:
	Transform mTransform;
	Matrix mProjectionMatrix;
	float mFarPlane;
	float mNearPlane;
	float mFov;
	float mAspectRatio;
	float mOrtoSize;
	Vector2 mOrtoResolution;
	bool mKeepAspect;
	CameraPerspective mPerspective;
};
