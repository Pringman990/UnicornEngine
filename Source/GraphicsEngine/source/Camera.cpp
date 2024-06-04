#include "GraphicsPch.h"
#include "Camera.h"

Camera::Camera()
	:
	mTransform(),
	mProjectionMatrix(),
	mNearPlane(0.01f),
	mFarPlane(1000.0f),
	mFov(90),
	mAspectRatio(16/9)
{
}

Camera::~Camera()
{
}

void Camera::SetPerspective(float aFovAngleY, float aAspectRatio, float aNearZ, float aFarZ)
{
	assert(aNearZ < aFarZ);
	assert(aNearZ > 0);
	assert(aAspectRatio > 0);
	assert(aFovAngleY > 0);

	mFarPlane = aFarZ;
	mNearPlane = aNearZ;
	mFov = aFovAngleY;
	mAspectRatio = aAspectRatio;
	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspectRatio, mNearPlane, mFarPlane);
}

Matrix Camera::GetClipSpaceMatrix()
{
	return mTransform.GetMatrix().Invert() * mProjectionMatrix;
}
