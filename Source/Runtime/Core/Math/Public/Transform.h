#pragma once
#include <SimpleMath.h>

class Transform
{
public:
	bool operator != (const Transform& V) const noexcept;
public:
	Transform() = default;
	Transform(Vector3 aPosition, Quaternion aQRotation, Vector3 aScale)
		:
		mPosition(aPosition),
		mQRotation(aQRotation),
		mScale(aScale)
	{
	};

	Transform(Vector3 aPosition, Vector3 aEularRotation, Vector3 aScale)
		:
		mPosition(aPosition),
		mQRotation(Quaternion::CreateFromYawPitchRoll(aEularRotation.y, aEularRotation.x, aEularRotation.z)),
		mScale(aScale)
	{
	};

	const Vector3 GetPosition() const { return mPosition; };
	const Vector3 GetScale() const { return mScale; };
	const Quaternion GetQuaternion() const { return mQRotation; };
	const Vector3 GetEularRotation() const { return mQRotation.ToEuler(); };

	void SetPosition(Vector3 aPosition) { mPosition = aPosition; };
	void SetScale(Vector3 aScale) { mScale = aScale; };
	void SetRotation(Quaternion aQRotation) { mQRotation = aQRotation; };
	void SetRotation(Vector3 aEularRotation)
	{
		mQRotation = Quaternion::CreateFromYawPitchRoll(aEularRotation.y, aEularRotation.x, aEularRotation.z);
	};

	Matrix GetMatrix() const
	{
		Matrix result = {};
		result *= Matrix::CreateScale(mScale);
		result *= Matrix::CreateFromQuaternion(mQRotation);
		result *= Matrix::CreateTranslation(mPosition);
		return result;
	}

private:
	Vector3 mPosition = {};
	Vector3 mScale = { 1,1,1 };
	Quaternion mQRotation = {};
};

inline bool Transform::operator != (const Transform& V) const noexcept
{
	return (mPosition != V.mPosition || mScale != V.mScale || mQRotation != V.mQRotation);
}