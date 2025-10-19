#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUBuffer.h"
#include "GPUTexture.h"
#include "Shader.h"

class LogicalDevice;
class Renderer;

struct ID3D11CommandList;

class CommandList
{
	friend class Renderer;
	friend class LogicalDevice;
public:
	CommandList();
	CommandList(ComPtr<ID3D11DeviceContext> DefferedContext, Renderer* InRenderer);

	void SetTopology(PrimitiveTopology Topology) const;
	void SetVertexBuffer(DirectResourceHandle<GPUBuffer> Buffer, uint32 Stride) const;
	void SetIndexBuffer(DirectResourceHandle<GPUBuffer> Buffer, RenderFormat Format = RenderFormat::R32_UINT) const;

	void UpdateConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, void* Data) const;
	void BindConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, uint32 Slot, ShaderStage Stages) const;
	void BindConstantBuffers(Vector<DirectResourceHandle<GPUConstantBuffer>> Buffers, uint32 StartSlot, ShaderStage Stages) const;

	void SetRenderTargets(Vector<GPUResourceHandle<GPUTexture>> Targets, GPUResourceHandle<GPUTexture> DepthStencil) const;

	void ClearRenderTarget(GPUResourceHandle<GPUTexture> Target, Color ClearColor) const;
	void ClearDepthStencil(GPUResourceHandle<GPUTexture> Target) const;

	void SetViewport(GPUResourceHandle<GPUTexture> Viewport);

	void SetShader(GPUResourceHandle<Shader> ShaderHandle) const;

	void DrawIndexed(uint32 IndexCount, uint32 StartIndex) const;

	ID3D11CommandList* Finish();
	void Reset();

private:
private:
	Renderer* mRenderer;
	ComPtr<ID3D11DeviceContext> mContext;
	ComPtr<ID3D11CommandList> mCachedList;
};