#pragma once
#include <Core.h>
#include "RenderGraph.h"

class RenderPassBuilder
{
public:
	RenderPassBuilder(RenderGraph::Pass& Pass) : mPass(Pass) {};

	//template<typename T>
	//ScopedHandle Read(GPUResourceHandle<T> Handle)
	//{
	//	ScopedHandle scopedHandle;
	//	scopedHandle.generation = Handle.generation;
	//	scopedHandle.index = Handle.index;
	//	scopedHandle.type = typeid(T);

	//	mPass.reads.push_back(scopedHandle);

	//	return scopedHandle;
	//};

	//template<typename T>
	//ScopedHandle Write(GPUResourceHandle<T> Handle)
	//{
	//	ScopedHandle scopedHandle;
	//	scopedHandle.generation = Handle.generation;
	//	scopedHandle.index = Handle.index;
	//	scopedHandle.type = typeid(T);

	//	mPass.writes.push_back(scopedHandle);

	//	return scopedHandle;
	//};

private:
	RenderGraph::Pass& mPass;
};