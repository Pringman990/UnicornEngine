#pragma once
#include <SimpleMath.h>

class Transform
{
public:
	bool operator != (const Transform& V) const noexcept;
public:
	Transform() : mPosition(Vector3(0, 0, 0)), mQRotation(Quaternion(0,0,0,0)), mScale(Vector3(1,1,1)) {};
	Transform(Vector3 Position, Quaternion QRotation, Vector3 Scale)
		:
		mPosition(Position),
		mQRotation(QRotation),
		mScale(Scale)
	{
	};

	Transform(Vector3 Position, Vector3 EularRotation, Vector3 Scale)
		:
		mPosition(Position),
		mQRotation(Quaternion::CreateFromYawPitchRoll(EularRotation.y, EularRotation.x, EularRotation.z)),
		mScale(Scale)
	{
	};

	const Vector3 GetPosition() const { return mPosition; };
	const Vector3 GetScale() const { return mScale; };
	const Quaternion GetQuaternion() const { return mQRotation; };
	const Vector3 GetEularRotation() const { return mQRotation.ToEuler(); };

	void SetPosition(Vector3 Position) { mPosition = Position; };
	void SetScale(Vector3 Scale) { mScale = Scale; };
	void SetRotation(Quaternion QRotation) { mQRotation = QRotation; };
	void SetRotation(Vector3 EularRotation)
	{
		mQRotation = Quaternion::CreateFromYawPitchRoll(EularRotation.y, EularRotation.x, EularRotation.z);
	};

	Matrix GetMatrix() const
	{
		Matrix result = {};
		result *= Matrix::CreateScale(mScale);
		result *= Matrix::CreateFromQuaternion(mQRotation);
		result *= Matrix::CreateTranslation(mPosition);
		return result;
	}

	static const Transform Identity;
private:
	Vector3 mPosition = {};
	Vector3 mScale = { 1,1,1 };
	Quaternion mQRotation = {};
};

inline bool Transform::operator != (const Transform& V) const noexcept
{
	return (mPosition != V.mPosition || mScale != V.mScale || mQRotation != V.mQRotation);
}