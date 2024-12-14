#include "pch.h"
#include "Camera.h"

Camera::Camera()
	:
	mTransform({}),
	mProjectionMatrix(Matrix()),
	mFarPlane(1000.f),
	mNearPlane(0.01f),
	mFov(90),
	mAspectRatio((16.f / 9.f))
{

}

Camera::~Camera()
{
}

void Camera::SetPerspective(float aFovAngleY, float aAspectRatio, float aNearZ, float aFarZ)
{
	_ENSURE_RENDERER(aNearZ < aFarZ, "Far plane cant be less then Near plane");
	_ENSURE_RENDERER(aNearZ > 0, "Near plane needs to be above Zero");
	_ENSURE_RENDERER(aAspectRatio > 0, "Aspect Ratio needs to be above Zero");
	_ENSURE_RENDERER(aFovAngleY > 0, "FOV needs to be above Zero");

	mFarPlane = aFarZ;
	mNearPlane = aNearZ;
	mFov = aFovAngleY;
	mAspectRatio = aAspectRatio;
	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspectRatio, mNearPlane, mFarPlane);
}

void Camera::SetOrthographic(Vector2 aResolution, float aNearZ, float aFarZ)
{
	_ENSURE_RENDERER(aNearZ < aFarZ, "Far plane cant be less then Near plane");
	_ENSURE_RENDERER(aNearZ > 0, "Near plane needs to be above Zero");
	_ENSURE_RENDERER(aResolution.x > 0, "Resolution X needs to be above Zero");
	_ENSURE_RENDERER(aResolution.y > 0, "Resolution Y needs to be above Zero");

	mFarPlane = aFarZ;
	mNearPlane = aNearZ;
	mProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0, aResolution.x, 0, aResolution.y, aNearZ, aFarZ);
}

Transform& Camera::GetTransform()
{
	return mTransform;
}

Matrix& Camera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

void Camera::GetFarNearPlanes(float& OUT aFar, float& OUT aNear)
{
	aFar = mFarPlane;
	aNear = mNearPlane;
}

Matrix Camera::GetClipSpaceMatrix()
{
	return mTransform.GetMatrix().Invert() * mProjectionMatrix;
}