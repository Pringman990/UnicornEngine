#pragma once
#include <EngineMinimal.h>

#include <GPUResources/GPUTexture.h>
#include <CommandList.h>
#include <RenderScene.h>

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

struct RenderPassContext
{
	RenderPassContext(CommandList& Command) : cmd(Command) {};

	CommandList& cmd;
	Vector<MeshInstance> scene;
};

struct RenderPass
{
	using PassExecuteFn = Func<void(RenderPassContext&)>;

	String name;
	//GPUResourceHandle<GPUTexture> target;

	PassExecuteFn execute;

	//Vector<RenderPassDependency> reads;
	Vector<RenderPassDependency> writes;
};