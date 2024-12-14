#pragma once
#include <Singleton.h>
#include <StandardTypes.h>
#include "IRenderingBackend.h"
#include "Buffers/IRenderBufferFactory.h"
#include "Shader/InputLayoutManager.h"
#include "RenderQueue.h"

class Camera;

struct GraphicsCardInfo
{
	std::string name = "";
	uint32 vendorID = 0;
	uint64 totalVideoMemoryInMB = 0;	//VRAM
	uint32 systemMemoryInMB = 0;		//For integrated GPU, the dedicated RAM memory for GPU
	uint32 sharedMemoryInMB = 0;		//Fallback RAM memory for the GPU if it runs out of VRAM
	uint64 currentVideoMemoryUsage = 0; 
	uint64 approxFreeVideoMemory = 0;
};

class Renderer final : public Singleton<Renderer> 
{
public:
	bool Init();
	void PreRender();
	void RenderToBackbuffer();
	void Present();

	uint32 GetDrawCalls() const;
	const GraphicsCardInfo& GetGraphicsCardInfo() const;
	void SetGraphicsCardInfo(const GraphicsCardInfo& aInfo);

	void AddDrawCall();
	void ResetDrawCalls();

	bool IsVsyncEnabled() const;
	void SetVsync(bool shouldVsync);

	IRenderingBackend* GetRenderingBackend();
	IRenderBufferFactory* GetRenderBufferFactory();
	InputLayoutManager* GetInputLayoutManager();
	RenderQueue* GetRenderQueue();

	void SetActiveCamera(Camera* aCamera);
	Camera* GetActiveCamera();

private:
	friend class Singleton<Renderer>;
	friend class RendererFactory;
	friend class EngineLoop;
	Renderer();
	~Renderer();

private:
	IRenderingBackend* mRenderingBackend;
	IRenderBufferFactory* mRenderBufferFactory;
	InputLayoutManager* mInputLayoutManager;

	RenderQueue* mRenderQueue;

	Camera* mActiveCamera;

	GraphicsCardInfo mGraphicsCardInfo;

	uint32 mDrawCalls;
	bool mVsyncEnabled;
};