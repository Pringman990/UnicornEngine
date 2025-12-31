#pragma once
#include "RendererDefines.h"
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
	RENDERER_API Camera();
	RENDERER_API virtual ~Camera();

	RENDERER_API void SetPerspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ);
	RENDERER_API void SetOrthographic(Vector2i Resolution, float NearZ, float FarZ);
	RENDERER_API void SetOrthoSize(float Size);

	RENDERER_API void KeepAspect(bool Keep) { mKeepAspect = Keep; };

	RENDERER_API Transform& GetTransform();
	RENDERER_API Vector3 GetPosition();
	RENDERER_API Matrix& GetProjectionMatrix();
	RENDERER_API Matrix GetViewMatrix();
	RENDERER_API float GetOrtoSize() const { return mOrtoSize; };

	RENDERER_API void GetFarNearPlanes(float& OUT Far, float& OUT Near);
	RENDERER_API Matrix GetClipSpaceMatrix();

	/**
	* Will not resize if KeepAspect is true.
	*/
	RENDERER_API void HandleResizeEvent(int32 Width, int32 Height);
protected:

protected:
	Transform mTransform;
	Matrix mProjectionMatrix;
	float mFarPlane;
	float mNearPlane;
	float mFov;
	float mAspectRatio;
	float mOrtoSize;
	Vector2i mOrtoResolution;
	bool mKeepAspect;
	CameraPerspective mPerspective;
};
