#pragma once
#include "Camera.h"

enum class FreeLookCameraControlSchema
{
	FreeLook, //FPS like
	OrthoPan //Top down/side scroller
};

class FreeLookCamera : public Camera
{
public:
	RENDERER_API FreeLookCamera();
	RENDERER_API ~FreeLookCamera() override;

	RENDERER_API void HandleInputsAndMove();
	RENDERER_API void SetControlSchema(FreeLookCameraControlSchema Schema) { mSchema = Schema; };

	RENDERER_API void SetMoveSpeed(float Speed) { mMoveSpeed = Speed; };
	RENDERER_API void SetBoostMoveMultiplier(float Speed) { mBoostMoveMultiplier = Speed; };
	RENDERER_API void SetRotateSpeed(float Speed) { mRotateSpeed = Speed; };
	RENDERER_API void SetZoomSpeed(float Speed) { mZoomSpeed = Speed; };

	RENDERER_API float GetMoveSpeed() const { return mMoveSpeed; };
	RENDERER_API float GetBoostMoveMultiplier() const { return mBoostMoveMultiplier; };
	RENDERER_API float GetRotateSpeed() const { return mRotateSpeed; };
	RENDERER_API float GetZoomSpeed() const { return mZoomSpeed; };

private:
	void HandleFreeLook();
	void HandleOrthoPan();
	void HandleZoom();

private:
	float mMoveSpeed;
	float mBoostMoveMultiplier;
	float mRotateSpeed;
	float mOrtoMouseSpeed;
	float mZoomSpeed;
	FreeLookCameraControlSchema mSchema;
};