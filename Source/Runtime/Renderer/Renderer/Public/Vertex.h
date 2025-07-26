#pragma once
#include <SimpleMath.h>

struct Vertex2D
{
	Vector2 position;
	Color color;
};

struct Vertex3D
{
	Vector4 position;
	Color color;
	Vector3 normal;
	Vector3 tangent;
	Vector3 bitangent;
	Vector2 uv;
};