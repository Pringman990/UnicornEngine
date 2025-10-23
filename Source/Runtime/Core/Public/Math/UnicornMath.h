#pragma once
#include "StandardTypes/StandardTypes.h"

struct Vector3i;

struct Vector2i
{
	Vector2i()
	{
		x = 0;
		y = 0;
	}

	Vector2i(int32 X, int32 Y)
	{
		x = X;
		y = Y;
	}

	int32 x;
	int32 y;

	operator Vector3i() const;
};

struct Vector3i
{
	Vector3i()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vector3i(int32 X, int32 Y, int32 Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	int32 x;
	int32 y;
	int32 z;

	operator Vector2i() const;
};

inline Vector2i::operator Vector3i() const
{
	return Vector3i(x, y, 0);
}

inline Vector3i::operator Vector2i() const
{
	return Vector2i(x, y);
}