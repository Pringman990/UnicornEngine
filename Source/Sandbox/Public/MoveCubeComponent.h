#pragma once
#include <Core.h>
#include <Sandbox/MoveCubeComponent.gen.h>

struct MoveCubeComponent
{
	float speed;
	GPUResourceHandle<struct GPUMesh> meshHandle;
};