#pragma once

struct ETransform
{
	Vector3 position = { 0,0,0 };
	Vector3 rotation = { 0,0,0 };
	Vector3 scale = { 1,1,1 };
};

REGISTER_COMPONENT(ETransform,
	REGISTER_MEMBER(ETransform, position)
	REGISTER_MEMBER(ETransform, rotation)
	REGISTER_MEMBER(ETransform, scale)
)