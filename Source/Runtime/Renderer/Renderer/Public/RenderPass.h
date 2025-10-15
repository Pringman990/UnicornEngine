#pragma once
#include <EngineMinimal.h>

#include <GPUTexture.h>

enum class ResourceAccess
{
	Write,
	Read
};

enum class LoadOp
{
	ClearDepth,
	ClearColor
};

struct RenderPassDependency
{
	GPUResourceHandle<GPUTexture> resourceHandle;
	ResourceAccess access;
	LoadOp loadOp;
};

struct RenderPass
{
	String name;
	Vector<RenderPassDependency> input;
	Vector<RenderPassDependency> output;
	Func<void(ID3D11DeviceContext* context)> execute;
};