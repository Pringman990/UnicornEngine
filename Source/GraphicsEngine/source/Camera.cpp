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
	SetPerspective(mFov, mAspectRatio, mNearPlane, mFarPlane);
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

void Camera::SetOrthographic(Vector2 aResolution, float aNearZ, float aFarZ)
{
	assert(aNearZ < aFarZ);
	assert(aNearZ > 0);
	assert(aResolution.x > 0);
	assert(aResolution.y > 0);

	mFarPlane = aFarZ;
	mNearPlane = aNearZ;
	mProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0, aResolution.x, 0, aResolution.y, aNearZ, aFarZ);
}

Matrix Camera::GetClipSpaceMatrix()
{
	return mTransform.GetMatrix().Invert() * mProjectionMatrix;
}
