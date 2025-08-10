#pragma once
#include <CommandBuffer.h>

#include <Sampler.h>

class IImguiBackend
{
public:
	IImguiBackend() {};
	virtual ~IImguiBackend() {};

	virtual bool Init() = 0;
	virtual void BeginFrame() = 0;
	virtual void RenderFrame() = 0;
	virtual void EndFrame(CommandBuffer* Buffer) = 0;

	//virtual void AddTextureToImgui(Texture2D* Texture, Sampler* Sample) = 0;

private:

};