#pragma once
#include "Camera.h"

class FreeLookCamera : public Camera
{
public:
	FreeLookCamera();
	~FreeLookCamera() override;

	void HandleInputsAndMove();

	void SetMoveSpeed(float Speed) { mMoveSpeed = Speed; };
	void SetBoostMoveSpeed(float Speed) { mBoostMoveSpeed = Speed; };
	void SetRotateSpeed(float Speed) { mRotateSpeed = Speed; };

	float GetMoveSpeed() const { return mMoveSpeed; };
	float GetBoostMoveSpeed() const { return mBoostMoveSpeed; };
	float GetRotateSpeed() const { return mRotateSpeed; };

private:
	float mMoveSpeed;
	float mBoostMoveSpeed;
	float mRotateSpeed;
};