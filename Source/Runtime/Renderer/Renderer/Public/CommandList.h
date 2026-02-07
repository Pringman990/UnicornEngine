#pragma once
#include "RendererDefines.h"
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUResources/GPUBuffer.h"
#include "GPUResources/GPUTexture.h"
#include "GPUResources/GPUMaterial.h"
#include "GPUResources/GPUMesh.h"
#include "Shader.h"
#include "Sampler.h"

class LogicalDevice;
class Renderer;

class Material;
class Mesh;

struct ID3D11CommandList;

class CommandList
{
	friend class Renderer;
	friend class LogicalDevice;
public:
	struct RenderCommand
	{
		enum class CommandType
		{
			DrawMesh
		};

		struct DrawSubMesh
		{
			GPUResourceHandle<GPUMesh> mesh;
			GPUResourceHandle<GPUMaterial> material;
			Transform transform;
			uint32 startIndex;
			uint32 indexCount;
			uint32 renderID = 0;
		};

		CommandType type;

		Variant<
			DrawSubMesh
		> data;
	};

	struct MapContext
	{
		void* data;
		uint32 rowPitch;
		uint32 depthPitch;
	};

public:
	RENDERER_API CommandList();
	RENDERER_API CommandList(ComPtr<ID3D11DeviceContext> DefferedContext, Renderer* InRenderer);
	RENDERER_API ~CommandList();

	//High Level

	RENDERER_API void DrawMesh(Mesh* MeshAsset, const Transform& MeshTransform);
	RENDERER_API void DrawMesh(Mesh* MeshAsset, const Transform& MeshTransform, uint32 RenderID);

	RENDERER_API ID3D11CommandList* Finalize();

	//Low Level
	RENDERER_API void SetSamplers(const Vector<Sampler*>& Samplers, uint32 StartSlot);

	RENDERER_API void SetTopology(PrimitiveTopology Topology) const;
	RENDERER_API void SetVertexBuffer(DirectResourceHandle<GPUBuffer> Buffer, uint32 Stride) const;
	RENDERER_API void SetIndexBuffer(DirectResourceHandle<GPUBuffer> Buffer, RenderFormat Format = RenderFormat::R32_UINT) const;

	RENDERER_API void UpdateConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, void* Data) const;
	RENDERER_API void BindConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, uint32 Slot, ShaderStageBind Stages) const;
	RENDERER_API void BindConstantBuffers(const Vector<DirectResourceHandle<GPUConstantBuffer>>& Buffers, uint32 StartSlot, ShaderStageBind Stages) const;

	RENDERER_API void SetRenderTargets(const Vector<GPUResourceHandle<GPUTexture>>& Targets, GPUResourceHandle<GPUTexture> DepthStencil) const;

	RENDERER_API void ClearRenderTarget(GPUResourceHandle<GPUTexture> Target, Color ClearColor) const;
	RENDERER_API void ClearDepthStencil(GPUResourceHandle<GPUTexture> Target) const;

	RENDERER_API void SetViewport(GPUResourceHandle<GPUTexture> Viewport) const;

	RENDERER_API void SetShaderProgram(GPUResourceHandle<ShaderProgram> ShaderHandle) const;

	RENDERER_API void SetShaderResources(const Vector<GPUResourceHandle<GPUTexture>>& Textures, uint32 StartSlot, ShaderStageBind Stages) const;
	RENDERER_API void SetShaderResource(GPUResourceHandle<GPUTexture> Texture, uint32 Slot, ShaderStageBind Stages) const;

	RENDERER_API void DrawIndexed(uint32 IndexCount, uint32 StartIndex) const;

	RENDERER_API ID3D11CommandList* Finish();
	RENDERER_API void Reset();

	RENDERER_API void CopyResource(GPUResourceHandle<GPUTexture> From, GPUResourceHandle<GPUTexture> To);
	RENDERER_API void Map(GPUResourceHandle<GPUTexture> Texture, MapContext& Context);
	RENDERER_API void Unmap(GPUResourceHandle<GPUTexture> Texture);

private:
	Renderer* mRenderer;
	ComPtr<ID3D11DeviceContext> mContext;
	ComPtr<ID3D11CommandList> mCachedList;

	Vector<RenderCommand> mCommandQueue;
};