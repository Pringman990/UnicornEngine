#pragma once
#include "RendererDefines.h"
#include "Camera.h"
#include "GPUResources/GPUTexture.h"

#include "RenderPass.h"

class RenderView
{
public:
	RENDERER_API RenderView();
	RENDERER_API ~RenderView();

	void SetCamera(SharedPtr<Camera> Camera) { mCamera = Camera; };

	RENDERER_API void AddPass(String Name, RenderPass::PassExecuteFn ExecuteFn);

	SharedPtr<Camera> GetCamera() { return mCamera; }
	Vector<RenderPass>& GetPasses() { return mPasses; }

private:
	SharedPtr<Camera> mCamera;
	Vector<RenderPass> mPasses;
};