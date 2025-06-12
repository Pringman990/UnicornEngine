#pragma once
#include <Core.h>

#include "EngineSubsystem.h"
#include "Camera.h"
#include "GraphicsCardInfo.h"
#include "Internal/IRHIRenderer.h"

class Renderer final : public EngineSubsystem<Renderer>
{
	friend class EngineSubsystem<Renderer>;
public:

	bool Init();
	void PreRender();
	void Present();

	uint32 GetDrawCalls() { return mDrawCalls; };
	const GraphicsCardInfo& GetGraphicsCardInfo() const { return mGraphicsCardInfo; };

private:
	Renderer();
	~Renderer();

	bool CreateRHI();
private:
	GraphicsCardInfo mGraphicsCardInfo;

	uint32 mDrawCalls;
	bool mVsync;

	//TODO: move to rendergraph or other, where the submit happens
	Camera* mActiveCamera;

	OwnedPtr<IRHIRenderer> mRHIRenderer;
};