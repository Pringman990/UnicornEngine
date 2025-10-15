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
	ENSURE(NearZ < FarZ, "Far plane cant be less then Near plane");
	ENSURE(NearZ > 0, "Near plane needs to be above Zero");
	ENSURE(AspectRatio > 0, "Aspect Ratio needs to be above Zero");
	ENSURE(FovAngleY > 0, "FOV needs to be above Zero");

	mFarPlane = FarZ;
	mNearPlane = NearZ;
	mFov = FovAngleY;
	mAspectRatio = AspectRatio;
	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspectRatio, mNearPlane, mFarPlane);

	//mProjectionMatrix(1, 1) *= -1; //Vulkan expects flipped Y axis
}

void Camera::SetOrthographic(Vector2 Resolution, float NearZ, float FarZ)
{
	ENSURE(NearZ < FarZ, "Far plane cant be less then Near plane");
	ENSURE(NearZ > 0, "Near plane needs to be above Zero");
	ENSURE(Resolution.x > 0, "Resolution X needs to be above Zero");
	ENSURE(Resolution.y > 0, "Resolution Y needs to be above Zero");

	mFarPlane = FarZ;
	mNearPlane = NearZ;
	mProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0, Resolution.x, 0, Resolution.y, NearZ, FarZ);

	//mProjectionMatrix(1, 1) *= -1; //Vulkan expects flipped Y axis
}

Transform& Camera::GetTransform()
{
	return mTransform;
}

Vector3 Camera::GetPosition()
{
	return mTransform.GetPosition();
}

Matrix& Camera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

Matrix Camera::GetViewMatrix()
{
	return mTransform.GetMatrix().Invert();
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