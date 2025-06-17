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

void Camera::SetPerspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
	_ENSURE_RENDERER(NearZ < FarZ, "Far plane cant be less then Near plane");
	_ENSURE_RENDERER(NearZ > 0, "Near plane needs to be above Zero");
	_ENSURE_RENDERER(AspectRatio > 0, "Aspect Ratio needs to be above Zero");
	_ENSURE_RENDERER(FovAngleY > 0, "FOV needs to be above Zero");

	mFarPlane = FarZ;
	mNearPlane = NearZ;
	mFov = FovAngleY;
	mAspectRatio = AspectRatio;
	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspectRatio, mNearPlane, mFarPlane);

	mProjectionMatrix(1, 1) *= -1; //Vulkan expects flipped Y axis
}

void Camera::SetOrthographic(Vector2 Resolution, float NearZ, float FarZ)
{
	_ENSURE_RENDERER(NearZ < FarZ, "Far plane cant be less then Near plane");
	_ENSURE_RENDERER(NearZ > 0, "Near plane needs to be above Zero");
	_ENSURE_RENDERER(Resolution.x > 0, "Resolution X needs to be above Zero");
	_ENSURE_RENDERER(Resolution.y > 0, "Resolution Y needs to be above Zero");

	mFarPlane = FarZ;
	mNearPlane = NearZ;
	mProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0, Resolution.x, 0, Resolution.y, NearZ, FarZ);

	mProjectionMatrix(1, 1) *= -1; //Vulkan expects flipped Y axis
}

Transform& Camera::GetTransform()
{
	return mTransform;
}

Matrix& Camera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

void Camera::GetFarNearPlanes(float& OUT Far, float& OUT Near)
{
	Far = mFarPlane;
	Near = mNearPlane;
}

Matrix Camera::GetClipSpaceMatrix()
{
	return mTransform.GetMatrix().Invert() * mProjectionMatrix;
}