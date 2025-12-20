#include "Camera.h"

#include <Application/Windows/WindowsApplication.h>
#include <Application/Windows/WindowsWindowInfo.h>
#include <Application/Application.h>

Camera::Camera()
	:
	mTransform({}),
	mProjectionMatrix(Matrix()),
	mFarPlane(1000.f),
	mNearPlane(0.01f),
	mFov(90),
	mAspectRatio((16.f / 9.f)),
	mOrtoSize(10),
	mKeepAspect(false),
	mPerspective(CameraPerspective::Undefined)
{
	WindowsApplication* app = static_cast<WindowsApplication*>(Application::Instance()->GetApplication());
	app->OnWindowResizeEvent.AddRaw(this, &Camera::HandleResizeEvent);
}

Camera::~Camera()
{
	WindowsApplication* app = static_cast<WindowsApplication*>(Application::Instance()->GetApplication());
	app->OnWindowResizeEvent.RemoveOwned(this);
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
	mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(mFov), mAspectRatio, mNearPlane, mFarPlane);

	//mProjectionMatrix(1, 1) *= -1; //Vulkan expects flipped Y axis
	mPerspective = Camera::CameraPerspective::Perspective;
}

void Camera::SetOrthographic(Vector2 Resolution, float NearZ, float FarZ)
{
	ENSURE(NearZ < FarZ, "Far plane cant be less then Near plane");
	ENSURE(NearZ > 0, "Near plane needs to be above Zero");
	ENSURE(Resolution.x > 0, "Resolution X needs to be above Zero");
	ENSURE(Resolution.y > 0, "Resolution Y needs to be above Zero");

	mOrtoResolution = Resolution;
	mFarPlane = FarZ;
	mNearPlane = NearZ;

	float orthoHeight = mOrtoSize; // world units
	float orthoWidth = orthoHeight * (mOrtoResolution.x / mOrtoResolution.y);

	float halfW = orthoWidth * 0.5f;
	float halfH = orthoHeight * 0.5f;

	mProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
		-halfW, +halfW,
		-halfH, +halfH,
		NearZ, FarZ
	);

	//mProjectionMatrix(1, 1) *= -1; //Vulkan expects flipped Y axis
	mPerspective = Camera::CameraPerspective::Orthographic;
}

void Camera::SetOrthoSize(float Size)
{
	mOrtoSize = Size;
	SetOrthographic(mOrtoResolution, mNearPlane, mFarPlane);
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

void Camera::HandleResizeEvent(int32 Width, int32 Height)
{
	if (mKeepAspect)
		return;

	switch (mPerspective)
	{
	case Camera::CameraPerspective::Perspective:
	{
		SetPerspective(mFov, ((float)Width / (float)Height), mNearPlane, mFarPlane);
		break;
	}
	case Camera::CameraPerspective::Orthographic:
	{
		SetOrthographic(Vector2(Width, Height), mNearPlane, mFarPlane);
		break;
	}
	default:
		LOG_WARNING("Camera was set to resize but has invalid perspective");
		break;
	}
}