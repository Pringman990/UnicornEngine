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
	Vector2 uv;
	Vector3 normal;
	Vector3 bitangent;
	Vector3 tangent;
};