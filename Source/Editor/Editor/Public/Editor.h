#pragma once
#include <Core.h>

class IImguiBackend;
class CommandBuffer;

class Texture2D;
class Sampler;

struct GPUTexture;

class Editor
{
public:
	Editor();
	~Editor();

	bool Init();
	void BeginFrame();
	void Render();
	void EndFrame(CommandBuffer* Buffer);

	void AddTextureToImgui(Texture2D* Texture);
	void AddTextureToImgui(GPUResourceHandle<GPUTexture> Handle);

	uint32 GetPreviousFrameDrawCalls() const { return mPreviousFrameDrawCalls; };

private:
	void RegisterEditorWindows();
private:
	IImguiBackend* mImguiBackend;

	Sampler* mTextureSampler;

	Vector<Texture2D*> mTextureUploadQueue;
	Vector<GPUResourceHandle<GPUTexture>> mGPUTextureUploadQueue;
	//UnorderedMap<GPUResourceHandle<GPUTexture>, ImTextureID> mGPUTextures;

	uint32 mPreviousFrameDrawCalls = 0;
};